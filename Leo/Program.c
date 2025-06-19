#include <process.h>
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>
#pragma comment(lib, "ws2_32.lib")
HANDLE ghMutex;
int puertos_seguros = 0;
int total_active_ports = 0;

typedef struct
{
    int Port;
    int *array;
    int MinPort;
    int Posc;
} ScanParams;

typedef struct
{
    int port;
    const char *service;
} PortService;

/*
  // Configurar estructura de conexión a la q se va a conectar el socket
    struct sockaddr_in server = {0};
    server.sin_family = AF_INET;                     // IPv4
    server.sin_addr.s_addr = inet_addr("127.0.0.1"); // ip en formato de red para leer a.b.c.d
    server.sin_port = htons(80);                     // Puerto en formato de red para q lo lea la computadora

    //   modo no bloqueante para permitir multiple conexion
    //   u_long modoNoBloqueante = 1;
    //   ioctlsocket(sock, FIONBIO, &modoNoBloqueante);
 */

// Lista de puertos comunes y sus servicios
PortService port_services[] = {
    //  Puertos bien conocidos (1-1024)
    {1, "TCPMUX"},
    {5, "RJE"},
    {7, "ECHO"},
    {9, "DISCARD"},
    {11, "SYSTAT"},
    {13, "DAYTIME"},
    {17, "QOTD"},
    {18, "MSP"},
    {19, "CHARGEN"},
    {20, "FTP-Data"},
    {21, "FTP"},
    {22, "SSH"},
    {23, "Telnet"},
    {25, "SMTP"},
    {37, "TIME"},
    {42, "WINS"},
    {43, "WHOIS"},
    {49, "TACACS"},
    {53, "DNS"},
    {67, "DHCP-Server"},
    {68, "DHCP-Client"},
    {69, "TFTP"},
    {70, "Gopher"},
    {79, "Finger"},
    {80, "HTTP"},
    {88, "Kerberos"},
    {102, "MS Exchange"},
    {110, "POP3"},
    {111, "RPCbind"},
    {113, "IDENT"},
    {119, "NNTP"},
    {123, "NTP"},
    {135, "MS-RPC"},
    {137, "NetBIOS-NS"},
    {138, "NetBIOS-DGM"},
    {139, "NetBIOS-SSN"},
    {143, "IMAP"},
    {161, "SNMP"},
    {162, "SNMP-Trap"},
    {177, "XDMCP"},
    {179, "BGP"},
    {194, "IRC"},
    {201, "AppleTalk"},
    {264, "BGMP"},
    {318, "TSP"},
    {381, "HP OpenView"},
    {383, "HP OpenView"},
    {389, "LDAP"},
    {411, "Direct Connect"},
    {412, "Direct Connect"},
    {427, "SLP"},
    {443, "HTTPS"},
    {445, "SMB"},
    {464, "Kerberos"},
    {465, "SMTPS"},
    {497, "Dantz"},
    {500, "IPSec/IKE"},
    {512, "rexec"},
    {513, "rlogin"},
    {514, "Syslog"},
    {515, "LPD"},
    {520, "RIP"},
    {521, "RIPng"},
    {540, "UUCP"},
    {548, "AFP"},
    {554, "RTSP"},
    {563, "NNTPS"},
    {587, "SMTP-Submission"},
    {591, "FileMaker"},
    {593, "MS DCOM"},
    {631, "IPP"},
    {636, "LDAPS"},
    {639, "MSDP"},
    {646, "LDP"},
    {691, "MS Exchange"},
    {860, "iSCSI"},
    {873, "rsync"},
    {902, "VMware Auth"},
    {989, "FTPS-DATA"},
    {990, "FTPS"},
    {993, "IMAPS"},
    {995, "POP3S"},
    {1023, "Reserved"},

    // Puertos registrados importantes (1024-49151)
    {1024, "Reserved"},
    {1025, "MS RPC"},
    {1026, "Windows Messenger"},
    {1027, "Windows Messenger"},
    {1028, "Windows Messenger"},
    {1029, "MS DCOM"},
    {1058, "nim"},
    {1059, "nimreg"},
    {1080, "SOCKS"},
    {1099, "RMI Registry"},
    {1109, "Kerberos POP"},
    {1167, "phone"},
    {1194, "OpenVPN"},
    {1214, "Kazaa"},
    {1241, "Nessus"},
    {1311, "Dell OpenManage"},
    {1337, "WASTE"},
    {1352, "Lotus Notes"},
    {1387, "cadsi-lm"},
    {1414, "IBM MQ"},
    {1433, "MS SQL"},
    {1434, "MS SQL Monitor"},
    {1494, "Citrix ICA"},
    {1512, "WINS"},
    {1521, "Oracle DB"},
    {1524, "ingreslock"},
    {1583, "Pervasive SQL"},
    {1589, "Cisco VQP"},
    {1604, "Citrix ICA"},
    {1645, "RADIUS"},
    {1701, "L2TP"},
    {1719, "H.323"},
    {1720, "H.323"},
    {1723, "PPTP"},
    {1755, "MMS"},
    {1801, "MSMQ"},
    {1812, "RADIUS"},
    {1863, "MSN"},
    {1900, "UPnP"},
    {1985, "Cisco HSRP"},
    {2000, "Cisco SCCP"},
    {2002, "Cisco ACS"},
    {2049, "NFS"},
    {2082, "cPanel"},
    {2083, "cPanel HTTPS"},
    {2100, "Oracle XDB"},
    {2222, "DirectAdmin"},
    {2302, "Halo"},
    {2483, "Oracle DB SSL"},
    {2484, "Oracle DB"},
    {2745, "Bagle.H"},
    {2967, "Symantec AV"},
    {3000, "Ruby on Rails"},
    {3050, "Interbase DB"},
    {3074, "Xbox Live"},
    {3128, "Squid"},
    {3222, "GLBP"},
    {3260, "iSCSI"},
    {3306, "MySQL"},
    {3389, "RDP"},
    {3689, "iTunes"},
    {3690, "Subversion"},
    {3724, "WoW"},
    {3784, "Ventrilo"},
    {4333, "mSQL"},
    {4444, "Metasploit"},
    {4500, "IPSec NAT-T"},
    {4567, "Sinatra"},
    {4662, "eMule"},
    {4672, "eMule"},
    {4899, "Radmin"},
    {5000, "UPnP"},
    {5001, "Synergy"},
    {5004, "RTP"},
    {5005, "RTP"},
    {5050, "Yahoo! Messenger"},
    {5060, "SIP"},
    {5190, "AIM"},
    {5222, "XMPP"},
    {5223, "XMPP SSL"},
    {5432, "PostgreSQL"},
    {5500, "VNC Server"},
    {5631, "pcAnywhere"},
    {5800, "VNC HTTP"},
    {5900, "VNC"},
    {6000, "X11"},
    {6112, "Blizzard"},
    {6129, "DameWare"},
    {6257, "WinMX"},
    {6346, "Gnutella"},
    {6500, "GameSpy"},
    {6566, "SANE"},
    {6588, "AnalogX"},
    {6665, "IRC"},
    {6666, "IRC"},
    {6667, "IRC"},
    {6668, "IRC"},
    {6669, "IRC"},
    {6679, "IRC SSL"},
    {6697, "IRC SSL"},
    {6881, "BitTorrent"},
    {6882, "BitTorrent"},
    {6883, "BitTorrent"},
    {6884, "BitTorrent"},
    {6885, "BitTorrent"},
    {6886, "BitTorrent"},
    {6887, "BitTorrent"},
    {6888, "BitTorrent"},
    {6889, "BitTorrent"},
    {6890, "BitTorrent"},
    {6891, "BitTorrent"},
    {6892, "BitTorrent"},
    {6901, "BitTorrent"},
    {6969, "BitTorrent"},
    {6970, "BitTorrent"},
    {7212, "GhostSurf"},
    {7648, "CU-SeeMe"},
    {8000, "HTTP-Alt"},
    {8008, "HTTP Alt"},
    {8080, "HTTP-Proxy"},
    {8081, "HTTP-Proxy"},
    {8087, "Parallels"},
    {8088, "HTTP-Alt"},
    {8090, "HTTP-Alt"},
    {8118, "Privoxy"},
    {8200, "VMware Server"},
    {8222, "VMware Server"},
    {8291, "Winbox"},
    {8294, "Blizzard"},
    {8333, "Bitcoin"},
    {8400, "Commvault"},
    {8443, "HTTPS-Alt"},
    {8530, "WSUS"},
    {8531, "WSUS"},
    {8888, "HTTP-Alt"},
    {9000, "PHP-FPM"},
    {9001, "Tor"},
    {9043, "WebSphere"},
    {9090, "HTTP-Alt"},
    {9100, "JetDirect"},
    {9119, "MXit"},
    {9293, "Sony PlayStation"},
    {9418, "Git"},
    {9535, "mRemoteNG"},
    {9800, "WebDAV"},
    {9898, "Dabber"},
    {9999, "Urchin"},
    {10000, "Webmin"},
    {10001, "Ubiquiti"},
    {10113, "NetIQ"},
    {10116, "NetIQ"},
    {11371, "OpenPGP"},
    {12035, "Second Life"},
    {12043, "Second Life"},
    {12203, "Second Life"},
    {12345, "NetBus"},
    {13720, "NetBackup"},
    {13721, "NetBackup"},
    {14567, "Battlefield"},
    {15118, "Dipnet"},
    {19226, "AdminSecure"},
    {19638, "Ensim"},
    {20000, "DNP3"},
    {24800, "Synergy"},
    {25999, "Xfire"},
    {27015, "Steam"},
    {27017, "MongoDB"},
    {27374, "Sub7"},
    {28960, "Call of Duty"},
    {31337, "Back Orifice"},
    {33434, "traceroute"},
    {37777, "Digital Video Recorder"},
    {0, NULL} // Marcador de fin
};

// Escanea los servicios comunes guardados a ver si el puerto en cuestion coincide con alguno de estos servicios guardados
const void get_service(int port)
{
    for (int i = 0; i < (int)(sizeof(port_services) / sizeof(port_services[0])); i++)
        if (port_services[i].port == port)
        {
            WaitForSingleObject(ghMutex, INFINITE);
            printf("Servicio asociado al puerto %d : %s \n", port, port_services[i].service);
            puertos_seguros++;
            ReleaseMutex(ghMutex); // Desbloquear mutex
            return;
        }
    printf("Servicio asociado al puerto %d : desconocido, potencialmente peligroso\n", port);
    ReleaseMutex(ghMutex); // Desbloquear mutex
}

int ScanPort(ScanParams scan)
{
    int resulta = 0;
    char host[256];

    //  Configurar estructura addrinfo
    struct addrinfo hints, *result = NULL;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC; // Soporta IPv4 e IPv6
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    WaitForSingleObject(ghMutex, INFINITE);
    char portstr[6];
    snprintf(portstr, sizeof(portstr), "%d", scan.Port);
    if (getaddrinfo(host, portstr, &hints, &result) != 0)
        printf("Error resolviendo direccion para el puerto %d\n", scan.Port);
    ReleaseMutex(ghMutex);

    WaitForSingleObject(ghMutex, INFINITE);
    SOCKET sock = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (sock == INVALID_SOCKET)
        printf("Error creando socket para el puerto %d: %d\n", scan.Port, WSAGetLastError());
    ReleaseMutex(ghMutex);

    // Optimizacion para q cuando el puerto este cerrado se bloquee antes
    DWORD timeout = 50; // 50ms
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char *)&timeout, sizeof(timeout));
    setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (const char *)&timeout, sizeof(timeout));

    int connectionResult = connect(sock, result->ai_addr, (int)result->ai_addrlen);
    if (connectionResult == SOCKET_ERROR)
    {
        WaitForSingleObject(ghMutex, INFINITE);
        int error = WSAGetLastError();
        scan.array[scan.Posc] = 0;
        ReleaseMutex(ghMutex); // Desbloquear mutex
        printf("No voy a marcar el puerto este -> %d en la poscion esta -> %d\n", scan.Port, scan.Posc);
        return -1;
        switch (error)
        {
        case WSAETIMEDOUT:
            printf("Puerto %d: TIMEOUT (filtrado)\n", scan.Port);
            break;

        case WSAECONNREFUSED:
            printf("Puerto %d: CERRADO, error : %d \n", scan.Port, WSAGetLastError());
            break;

        default:
            printf("Puerto %d: Error %d\n", scan.Port, error);
            break;
        }
    }
    else
    {
        printf("Voy a marcar el puerto este -> %d en la poscion esta -> %d\n", scan.Port, scan.Posc);
        WaitForSingleObject(ghMutex, INFINITE);
        scan.array[scan.Posc] = 1;
        total_active_ports++;
        ReleaseMutex(ghMutex);
    }

    //  Liberar recursos
    closesocket(sock);
    freeaddrinfo(result);
    return 0;
}

void scan_thread(void *param)
{
    ScanParams port = *(ScanParams *)param;
    ScanPort(port);
    free(param);
}

int main()
{
    int start_port = 1, end_port = 1024;
    puertos_seguros = 0;
    total_active_ports = 0;

    // Inicializar Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        printf("Error inicializando Winsock : %d\n", WSAGetLastError());
        return 1;
    }

    /*
        printf("Empieza el escaner\n");
        printf("Ingrese puerto inicial (ej: 80): \n");
        scanf("%d", &start_port);
        printf("Ingrese puerto final (ej: 100): \n");
        scanf("%d", &end_port);
        */

    ghMutex = CreateMutex(NULL, FALSE, NULL);
    if (ghMutex == NULL)
    {
        printf("Error al crear el mutex\n");
        WSACleanup();
        return -1;
    }

    int *orden = malloc((end_port - start_port + 1) * sizeof(int));
    HANDLE *hThreads = malloc((end_port - start_port + 2) * sizeof(HANDLE));
    for (int port = start_port, i = 1; port <= end_port; port++, i++)
    {
        ScanParams *scan = malloc(sizeof(ScanParams));
        scan->Port = port;
        scan->MinPort = start_port;
        scan->array = orden;
        scan->Posc = i;
        hThreads[i] = (HANDLE)_beginthread(scan_thread, 0, scan);
        if (hThreads[i] == NULL)
        {
            printf("Error al crear hilo para puerto %d\n", port);
            free(scan);
        }
    }
    WaitForMultipleObjects(end_port - start_port + 1, hThreads, TRUE, INFINITE);

    for (int i = 1, port = start_port; i < sizeof(orden) / sizeof(orden[0]); i++, port++)
        if (orden[i])
            get_service(port);

    printf("Tota de puertos escaneados : %d\n", end_port - start_port);
    printf("Total de puertos abiertos : %d\n", total_active_ports);
    printf("Total de puertos cerrados : %d\n", end_port - start_port - total_active_ports);
    printf("Puertos potencialmente seguros : %d\n", puertos_seguros);
    printf("Puertos potencialmente comprometidos : %d\n", total_active_ports - puertos_seguros);

    // system("netstat -an | findstr \"LISTENING\"");

    free(orden);
    free(hThreads);
    CloseHandle(ghMutex);
    WSACleanup();
    return 0;
}
