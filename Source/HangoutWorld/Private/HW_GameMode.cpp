#include "HW_GameMode.h"
#include "HW_Character.h"
#include "HW_GameState.h"
#include "HW_PlayerController.h"
#include "HW_PlayerState.h"

AHW_GameMode::AHW_GameMode()
{
    DefaultPawnClass = AHW_Character::StaticClass();
    PlayerControllerClass = AHW_PlayerController::StaticClass();
    PlayerStateClass = AHW_PlayerState::StaticClass();
    GameStateClass = AHW_GameState::StaticClass();
}
