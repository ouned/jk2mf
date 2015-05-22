using System;
using System.Collections.Generic;
using System.Text;
using System.Net.Sockets;
using System.Threading;

namespace JK2MFServerlist
{
    class Program
    {
        static string validNameChars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890^$&/()=?!'+#*.,:<>|{[]}-_@ ";
        static int remaining;
        static Mutex mut = new Mutex();

        public struct Server
        {
            public string ip;
            public int port;
        }

        static UInt16 ReverseBytes(UInt16 value)
        {
            return (UInt16)((value & 0xFFU) << 8 | (value & 0xFF00U) >> 8);
        }

        // ----------------
        // SendRecv
        // Send req to ip and port.
        // Returns the lenght of bytes received and saves the data into buffer
        // ----------------
        static int SendRecv(string ip, int port, string req, byte[] buffer, int timeout = 1500)
        {
            // Set Timout
            Socket sock = new Socket(AddressFamily.InterNetwork, SocketType.Dgram, ProtocolType.Udp);
            sock.ReceiveTimeout = timeout;
            sock.SendTimeout = timeout;

            try
            {
                // Connect and send the request
                sock.Connect(ip, port);
                byte[] realReq = System.Text.Encoding.Default.GetBytes(req);
                sock.Send(realReq, 0, req.Length, SocketFlags.None);

                // Receive the answere
                int count = sock.Receive(buffer, 0, buffer.Length, SocketFlags.None);
                return count;
            }
            catch { return 0; }
        }

        // ----------------
        // GetJK2Servers
        // Requests the serverlist and saves the result in a Server struct array
        // ----------------
        static Server[] GetJK2Servers(int protocol)
        {
            byte[] rawList = new byte[2048];
            int rawLenght = SendRecv("jk2.ouned.de", 28060, "ÿÿÿÿgetservers " + protocol, rawList, 2000);

            // Lenght of bytes received - 25 (ÿÿÿÿgetserversResponse + \)
            int numServers = (rawLenght - 25) / 7;

            Server[] srvList = new Server[numServers];
            for (int i = 0; i < numServers; i++)
            {
                int srvPos = i * 7 + 25;

                // First 4 bytes are the IPAddress
                srvList[i].ip = String.Format("{0}.{1}.{2}.{3}", rawList[srvPos], rawList[srvPos + 1], rawList[srvPos + 2], rawList[srvPos + 3]);

                // Following 2 bytes are the port (UInt16), BigEndian
                srvList[i].port = ReverseBytes(BitConverter.ToUInt16(rawList, srvPos + 4));
            }

            return srvList;
        }

        static string GetJK2ServerInfo(string ip, int port, int timeout)
        {
            byte[] rawRecv = new byte[4096];
            int count = SendRecv(ip, port, "ÿÿÿÿgetstatus", rawRecv, timeout);

            string recvStr = Encoding.Default.GetString(rawRecv, 0, count);

            if (recvStr.Contains("UG"))
            {
                int i = 5;
            }

            return recvStr;
        }

        static string InfoValueForKey(string info, string key)
        {
            try
            {
                int startPos = info.IndexOf("\\" + key + "\\");
                if (startPos == -1)
                    return string.Empty;

                startPos += ("\\" + key + "\\").Length;
                int endPos = info.IndexOf('\\', startPos + 1);
                if ( endPos == -1 )
                    endPos = info.IndexOf('\n', startPos + 1);

                return info.Substring(startPos, endPos - startPos);
            }
            catch { return string.Empty; }
        }

        static string FitToLenght(string str, int len)
        {
            int spaceAdd = len - str.Length;

            for (int i = 0; i < spaceAdd; i++)
            {
                str += ' ';
            }

            return str;
        }

        static void FitToLenght(int printLen, int reqLen)
        {
            int spaceAdd = reqLen - printLen;

            for (int i = 0; i < spaceAdd; i++)
            {
                Console.Write(' ');
            }
        }

        class Worker
        {
            public string ip;
            public int port;

            public void Work()
            {
                string srvInfo = GetJK2ServerInfo(ip, port, 1800);
                string srvMFVer = InfoValueForKey(srvInfo, "JK2MF");

                mut.WaitOne();

                // Does this server even run JK2MF?


                Console.Write(FitToLenght(ip + ":" + port, 22));

                // Print name including colors
                string srvName = InfoValueForKey(srvInfo, "sv_hostname");
                int charsPrinted = 0;
                for (int pos = 0; pos < srvName.Length; pos++)
                {
                    if (srvName[pos] == '^' && pos + 2 < srvName.Length && srvName[pos + 1] >= '1' && srvName[pos + 1] <= '7')
                    {
                        switch (srvName[pos + 1])
                        {
                            case '1':
                                Console.ForegroundColor = ConsoleColor.Red;
                                break;
                            case '2':
                                Console.ForegroundColor = ConsoleColor.Green;
                                break;
                            case '3':
                                Console.ForegroundColor = ConsoleColor.Yellow;
                                break;
                            case '4':
                                Console.ForegroundColor = ConsoleColor.DarkBlue;
                                break;
                            case '5':
                                Console.ForegroundColor = ConsoleColor.Cyan;
                                break;
                            case '6':
                                Console.ForegroundColor = ConsoleColor.Magenta;
                                break;
                            case '7':
                                Console.ForegroundColor = ConsoleColor.White;
                                break;
                        }

                        pos++;
                    }
                    else
                    {
                        bool found = false;
                        for (int j = 0; j < validNameChars.Length; j++)
                        {
                            if (validNameChars[j] == srvName[pos])
                                found = true;
                        }

                        if (found == true)
                            Console.Write(srvName[pos]);
                        else
                            Console.Write('.');

                        charsPrinted++;
                    }
                }

                Console.ForegroundColor = ConsoleColor.White;
                FitToLenght(charsPrinted, 34);

                Console.Write(FitToLenght(srvMFVer, 17));

                if (InfoValueForKey(srvInfo, "jk2mf_anticheat") == "1")
                    Console.Write("Yes ");
                else
                    Console.Write("No  ");

                if (InfoValueForKey(srvInfo, "jk2mf_httpdownloads") == "1")
                    Console.Write("Yes");
                else
                    Console.Write("No ");

                remaining--;
                mut.ReleaseMutex();
            }
        }

        static void Main(string[] args)
        {
            Console.ForegroundColor = ConsoleColor.White;
            Server[] masterList = GetJK2Servers(16);

            remaining = masterList.Length;
            for (int i = 0; i < masterList.Length; i++)
            {
                Worker workerObject = new Worker();
                workerObject.ip = masterList[i].ip;
                workerObject.port = masterList[i].port;
                Thread workerThread = new Thread(workerObject.Work);
                workerThread.Start();
            }

            while (true)
            {
                Thread.Sleep(50);

                mut.WaitOne();
                if (remaining == 0)
                    break;

                mut.ReleaseMutex();
            }
            mut.ReleaseMutex();

            Console.WriteLine("Press any key to continue...");
            Console.ReadKey();
        }
    }
}
