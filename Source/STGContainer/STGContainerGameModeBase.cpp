// Fill out your copyright notice in the Description page of Project Settings.

#include "STGContainerGameModeBase.h"
#include "Runtime/Json/Public/Dom/JsonObject.h"
#include "Runtime/Json/Public/Serialization/JsonReader.h"
#include "Runtime/Json/Public/Serialization/JsonSerializer.h"

ASTGContainerGameModeBase::ASTGContainerGameModeBase()
{
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.bCanEverTick = true;
}

void ASTGContainerGameModeBase::StartPlay()
{
	Super::StartPlay();

	auto* WebSocketModule = FModuleManager::LoadModulePtr< FWebSocketsModule>("WebSockets");
	WebSocket = WebSocketModule->CreateWebSocket("ws://127.0.0.1:7379/.json");
	
	WebSocket->OnConnected().AddLambda([]() {
		UE_LOG(LogTemp, Display, TEXT("WebSocket: Connected"));
	});

	WebSocket->OnMessage().AddLambda([&](FString Message) {
		UE_LOG(LogTemp, Display, TEXT("WebSocket: %s"), *Message);
		InWebSocketMessageQueue.Enqueue(Message);
		bAwaitingMessageReply = false;
	});

	WebSocket->Connect();

	// Test Set Value
	OutWebSocketMessageQueue.Enqueue(R"(["SET", "hello", "unreal"])");

	// Test Get Value
	OutWebSocketMessageQueue.Enqueue(R"(["GET", "hello"])");
}

void ASTGContainerGameModeBase::BeginDestroy()
{
	Super::BeginDestroy();

	if (WebSocket.IsValid())
	{
		WebSocket->Close();
	}
}

void ASTGContainerGameModeBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	FString Message;
	if (!InWebSocketMessageQueue.IsEmpty())
	{
		InWebSocketMessageQueue.Dequeue(Message);

		TSharedPtr<FJsonValue> JsonObject;
		auto const& JsonReader = TJsonReaderFactory<>::Create(Message);
		if (FJsonSerializer::Deserialize(JsonReader, JsonObject) && JsonObject.IsValid())
		{
			//JsonObject->
		}
	}

	if (!OutWebSocketMessageQueue.IsEmpty() && !bAwaitingMessageReply)
	{
		FString Message;
		OutWebSocketMessageQueue.Dequeue(Message);
		WebSocket->Send(Message);
		bAwaitingMessageReply = true;
	}
}