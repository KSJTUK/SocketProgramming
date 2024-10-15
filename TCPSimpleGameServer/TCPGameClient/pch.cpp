#include "pch.h"
#include "GameFramework.h"
#include "ServerService.h"

GameFramework gGameFramework;
ServerService gServerService;

std::shared_ptr<PointShape> Shapes::gPointShape = std::make_shared<PointShape>();
std::shared_ptr<Square> Shapes::gSquare = std::make_shared<Square>();