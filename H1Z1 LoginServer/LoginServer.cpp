#include "DisconnectReasons.hpp"
#include "ClientState.hpp"
#include "LoginServer.hpp"
#include "H1Z1.hpp"
#include "loguru.hpp"

H1Z1 hServer;

bool Send(SOCKET a, const char* b, const sockaddr* c, int d) {
	sendto(a, (const char*)b, sizeof(b),
		0, (const struct sockaddr*) & c,
		d);
	return true;
}

void c_h1z1_loginserver::OnMessage(SOCKET socket, struct sockaddr_in client_information, int client_lenght, unsigned char* received_data, int received_bytes)
{
	hServer.Init(socket, received_data);
	static bool bOnce;

#ifdef TESTMODE
	hServer.Hexdump((unsigned char*)received_data, received_bytes);
#endif
	
	if (!bOnce) {
		bOnce = true;

		if (hServer.IsClientProtocolSupported()) { // Checking if the client game version is supported
			LOG_F(INFO, "[Server] Client version correct");
		}
		else
		{
			LOG_F(WARNING, "[Server] Client version incorrect, disconnecting the client");
			if (closesocket(socket)) { // Disconnect the client here
				LOG_F(INFO, "[Server] Disconnected the client.");
			}
			/*else
			{
				LPSTR errString = NULL;
				FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, 0, WSAGetLastError(), 0, (LPSTR)& errString, 0, 0);

				LOG_F(ERROR, "[Winsock] unknown error occured");
				LocalFree(errString);
			}*/

			return;
		}
	}

	// Here we handle the received data from the client
	if (hServer.IsEqual(LoginRequestPattern, received_data)) { // Checking if the data looks like the Connection Request pattern

		LOG_F(INFO, "[Client] Connection request from %s", inet_ntoa(client_information.sin_addr));
		LOG_F(INFO, "[Server] Sending a reply to %s", inet_ntoa(client_information.sin_addr));

		// Seems like the game generate a UUID or a key used to identify him on the server-side, here it is
		LoginReply[2] = received_data[6];
		LoginReply[3] = received_data[7];
		LoginReply[4] = received_data[8];
		LoginReply[5] = received_data[9];

		//Now we reply to the client with his identifier
		if (sendto(socket, (const char*)LoginReply, sizeof(LoginReply), 0, (const struct sockaddr*) & client_information, client_lenght))
			LOG_F(INFO, "[Server] Reply sent to %s", inet_ntoa(client_information.sin_addr));

	}
}
