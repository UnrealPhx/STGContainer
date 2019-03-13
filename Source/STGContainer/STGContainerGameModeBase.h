// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "WebSockets/Public/WebSocketsModule.h"
#include "IWebSocket.h"
#include "Runtime/Core/Public/Containers/Queue.h"
#include "STGContainerGameModeBase.generated.h"

UCLASS()
class STGCONTAINER_API ASTGContainerGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	ASTGContainerGameModeBase();

	virtual void StartPlay() override;

	virtual void BeginDestroy() override;

	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable, Category = "WebSocket")
	void SendPlayerEvent(FString Event, FString Id, FString Value);

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AActor> AsteroidClass;

protected:
	TSharedPtr<IWebSocket> WebSocket;
	bool bWantsReconnect;

	TQueue<FString> ServerMessages;

	FString ServerName;
	FString ServerPort;
};
