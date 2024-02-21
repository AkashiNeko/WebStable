#pragma once
#ifndef WEBSTABLE_APP_VERSION_H
#define WEBSTABLE_APP_VERSION_H

#include "nanonet.h"

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

#define WEBSTABLE_VERSION 1.0
#define NANONET_VERSION __NANONET__

#endif // WEBSTABLE_APP_VERSION_H