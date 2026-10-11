#include "ARPGGameMode.h"
#include "ARPGPlayerController.h"
#include "ARPGPlayerState.h"
#include "GameFramework/Pawn.h"
#include "UObject/ConstructorHelpers.h"

AARPGGameMode::AARPGGameMode()
{
    PlayerControllerClass = AARPGPlayerController::StaticClass();
    PlayerStateClass = AARPGPlayerState::StaticClass();
    static ConstructorHelpers::FClassFinder<APawn> Player(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
    if (Player.Succeeded()) DefaultPawnClass = Player.Class;
}
