#ifndef NETWORK_H
#define NETWORK_H


// Core namespace
#include "Core/Init.hpp"
#include "Core/Errors.hpp"
#include "Core/ArgumentException.hpp"
#include "Core/InitialisationException.hpp"
#include "Core/ByteArray.hpp"

// System namespace
#include "System/Mutex.hpp"
#include "System/Semaphore.hpp"
#include "System/ThreadException.hpp"
#include "System/Thread.hpp"

// RegularExpressions namespace
#include "Text/Match.hpp"
#include "Text/Group.hpp"
#include "Text/Regex.hpp"

// Events namespace
#include "Events/EventHandlerBase.hpp"
#include "Events/EventHandler.hpp"
#include "Events/Event.hpp"
#include "Events/EventArgs.hpp"

// Network namespace
#include "Network/IpAddress.hpp"
#include "Network/Errors.hpp"
#include "Network/TcpSocket.hpp"
#include "Network/IpEndPoint.hpp"
#include "Network/Socket.hpp"
#include "Network/SocketSet.hpp"
#include "Network/NetworkException.hpp"
#include "Network/UdpSocket.hpp"
#include "Network/Dns.hpp"


#endif


