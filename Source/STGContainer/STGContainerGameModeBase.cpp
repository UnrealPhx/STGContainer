// Fill out your copyright notice in the Description page of Project Settings.

#include "STGContainerGameModeBase.h"

void ASTGContainerGameModeBase::StartPlay()
{
	Super::StartPlay();

	auto* WebSocketModule = FModuleManager::LoadModulePtr< FWebSocketsModule>("WebSockets");
	WebSocket = WebSocketModule->CreateWebSocket("ws://127.0.0.1:7379/.json");
	
	WebSocket->OnConnected().AddLambda([]() {
		UE_LOG(LogTemp, Display, TEXT("WebSocket: Connected"));
	});

	WebSocket->OnMessage().AddLambda([](FString Message) {
		UE_LOG(LogTemp, Display, TEXT("WebSocket: %s"), *Message);
	});

	WebSocket->Connect();

	WebSocket->Send(R"(["SET", "hello", "unreal"])");
}

void ASTGContainerGameModeBase::BeginDestroy()
{
	Super::BeginDestroy();

	if (WebSocket.IsValid())
	{
		WebSocket->Close();
	}
}