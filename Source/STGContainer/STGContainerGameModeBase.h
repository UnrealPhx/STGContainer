// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "WebSockets/Public/WebSocketsModule.h"
#include "IWebSocket.h"
#include "STGContainerGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class STGCONTAINER_API ASTGContainerGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	virtual void StartPlay() override;

	virtual void BeginDestroy() override;

protected:
	TSharedPtr<IWebSocket> WebSocket;
};
