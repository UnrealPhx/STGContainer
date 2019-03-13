// Fill out your copyright notice in the Description page of Project Settings.

#include "STGContainerGameModeBase.h"
#include "Misc/CommandLine.h"
#include "Runtime/Json/Public/Dom/JsonObject.h"
#include "Runtime/Json/Public/Serialization/JsonReader.h"
#include "Runtime/Json/Public/Serialization/JsonSerializer.h"

ASTGContainerGameModeBase::ASTGContainerGameModeBase()
{
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.bCanEverTick = true;
}

void ASTGContainerGameModeBase::SendPlayerEvent(FString Event, FString Id, FString Value)
{
	if (WebSocket.IsValid())
	{
		WebSocket->Send(FString::Printf(TEXT(R"({ "version": 1, "type": "player", "message": { "event": "%s", "id": "%s", "value": "%s" }})"), *Event, *Id, *Value));
	}
}

void ASTGContainerGameModeBase::StartPlay()
{
	Super::StartPlay();

	FString WebsocketAddress = "127.0.0.1:7379";
	if (FParse::Value(FCommandLine::Get(), TEXT("ws"), WebsocketAddress)) {
		WebsocketAddress = WebsocketAddress.Replace(TEXT("="), TEXT("")).Replace(TEXT("\""), TEXT(""));
	}
	WebsocketAddress.Split(":", &ServerName, &ServerPort);

	auto* WebSocketModule = FModuleManager::LoadModulePtr<FWebSocketsModule>("WebSockets");
	WebSocket = WebSocketModule->CreateWebSocket(FString::Printf(TEXT("ws://%s"), *WebsocketAddress));
	
	WebSocket->OnConnected().AddLambda([this, ref = FWeakObjectPtr(this)]() {
		if (!ref.IsValid()) return;

		UE_LOG(LogTemp, Display, TEXT("WebSocket: Connected"));

		auto NetMode = GetNetMode();
		if (NetMode == ENetMode::NM_ListenServer || NetMode == ENetMode::NM_DedicatedServer)
		{
			WebSocket->Send(R"({ "version": 1, "type": "status", "message": { "status": "online" }})");
		}
		else if (NetMode == ENetMode::NM_Standalone)
		{
			WebSocket->Send(R"({ "version": 1, "type": "lookup", "message": { "query": "server" }})");
		}
		bWantsReconnect = false;
	});

	WebSocket->OnConnectionError().AddLambda([this, ref = FWeakObjectPtr(this)](FString Error) {
		if (!ref.IsValid()) return;

		UE_LOG(LogTemp, Display, TEXT("WebSocket: %s"), *Error);
		bWantsReconnect = true;
	});

	WebSocket->OnMessage().AddLambda([this, ref = FWeakObjectPtr(this)](FString Message) {
		if (!ref.IsValid()) return;

		UE_LOG(LogTemp, Display, TEXT("WebSocket: %s"), *Message);
		ServerMessages.Enqueue(Message);
	});

	WebSocket->Connect();
}

void ASTGContainerGameModeBase::BeginDestroy()
{
	Super::BeginDestroy();

	if (WebSocket.IsValid())
	{
		WebSocket->Send(R"({ "version": 1, "type": "status", "message": { "status": "offline" }})");

		WebSocket->OnClosed().Clear();
		WebSocket->OnConnected().Clear();
		WebSocket->OnConnectionError().Clear();
		WebSocket->OnMessage().Clear();
		WebSocket->OnRawMessage().Clear();
		WebSocket->Close();
	}
}

void ASTGContainerGameModeBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// TODO: Throttle this reconection attempt
	if (bWantsReconnect && !WebSocket->IsConnected())
	{
		WebSocket->Connect();
	}

	const int32 MAX_MESSAGE_PARSE = 32;
	int32 MessagesParsed = 0;
	FString Message;
	while (!ServerMessages.IsEmpty() && MessagesParsed < MAX_MESSAGE_PARSE)
	{
		ServerMessages.Dequeue(Message);
		MessagesParsed++;

		TSharedPtr<FJsonObject> JsonObject;
		auto const& JsonReader = TJsonReaderFactory<>::Create(Message);
		if (FJsonSerializer::Deserialize(JsonReader, JsonObject) && JsonObject.IsValid())
		{
			bool WellFormed = true;

			int32 Version = 1;
			WellFormed &= JsonObject->TryGetNumberField("version", Version);

			FString Type;
			WellFormed &= JsonObject->TryGetStringField("type", Type);

			if (!WellFormed) continue;

			if (Type.Compare("lookup") == 0)
			{
				const TSharedPtr<FJsonObject>* Payload;
				WellFormed &= JsonObject->TryGetObjectField("message", Payload);
				if (!WellFormed) continue;

				FString ServerIp;
				WellFormed &= (*Payload)->TryGetStringField("ip", ServerIp);
				if (!WellFormed) continue;

				if (ServerIp.IsEmpty())
				{
					GetWorld()->ServerTravel(TEXT("Default?listen"));
				}
				else
				{
					GetWorld()->GetFirstPlayerController()->ClientTravel(ServerName, ETravelType::TRAVEL_Absolute);
				}
			}

			if (Type.Compare("spawn") == 0)
			{
				const TSharedPtr<FJsonObject>* Payload;
				WellFormed &= JsonObject->TryGetObjectField("message", Payload);
				if (!WellFormed) continue;

				FString ActorName;
				WellFormed &= (*Payload)->TryGetStringField("id", ActorName);
				if (!WellFormed) continue;
				
				FActorSpawnParameters SpawnParams;
				SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
				GetWorld()->SpawnActor<AActor>(AsteroidClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
			}
		}
	}
}