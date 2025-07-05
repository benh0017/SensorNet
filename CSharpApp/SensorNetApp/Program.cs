using System;
using System.Collections.Generic;
using System.Threading;

namespace SensorNetApp
{
    internal class Program
    {
        static void Main(string[] args)
        {
            var serial = new SerialManager();
            bool continuous = false;

            // List available COM ports
            string[] ports = System.IO.Ports.SerialPort.GetPortNames();
            Console.WriteLine("Available COM ports:");
            if (ports.Length == 0)
            {
                Console.WriteLine("  (None found — make sure your device is connected)");
            }
            else
            {
                foreach (string p in ports)
                {
                    Console.WriteLine(" " + p);
                }
            }
            Console.WriteLine();
            Console.Write("Enter COM port (ex, COM5): ");
            string port = Console.ReadLine();

            if (!serial.Connect(port))
            {
                Console.WriteLine("COM port busy.");
                Console.WriteLine("Press any key to exit...");
                Console.ReadKey();
                return;
            }

            Console.WriteLine($"\nConnected to {port}.");
            Console.WriteLine("Connection established.");

            bool running = true;
            while (running)
            {
                ShowMenu();

                Console.Write("> ");
                string input = Console.ReadLine()?.Trim();

                switch (input)
                {
                    case "1":
                        continuous = true;
                        HandleContinuousCommand(serial);
                        break;

                    case "2":
                        serial.Send("STOP");
                        continuous = false;
                        break;

                    case "3":
                        if (continuous)
                        {
                            Console.WriteLine("Cannot run TEST while logging is active. Use STOP first.");
                            break;
                        }
                        serial.Send("SINGLE");
                        break;

                    case "4":
                        if (continuous)
                        {
                            Console.WriteLine("Cannot run READ while logging is active. Use STOP first.");
                            break;
                        }
                        HandleReadCommand(serial);
                        break;

                    case "0":
                        running = false;
                        break;

                    default:
                        Console.WriteLine("Invalid option. Please choose a number from the menu.");
                        continue;
                }

                Thread.Sleep(500); // Allow Arduino time to respond

                while (true)
                {
                    string line = serial.ReadLine();
                    if (string.IsNullOrEmpty(line)) break;
                    Console.WriteLine("[ARDUINO] " + line);
                }
            }

            serial.Disconnect();
            Console.WriteLine("Press any key to exit...");
            Console.ReadKey();
        }

        static void ShowMenu()
        {
            Console.WriteLine("\nSelect a command:");
            Console.WriteLine(" 1 - START   (start continuous logging)");
            Console.WriteLine(" 2 - STOP    (stop logging)");
            Console.WriteLine(" 3 - TEST    (take a single reading)");
            Console.WriteLine(" 4 - READ    (Select csv file to read from)");
            Console.WriteLine(" 0 - Exit");
        }

        static void HandleContinuousCommand(SerialManager serial)
        {
            string dateCode = DateTime.Now.ToString("yyMMdd");

            Console.Write("Enter scan interval in seconds (1 - 86400): ");
            if (!int.TryParse(Console.ReadLine(), out int interval) || interval < 1 || interval > 86400)
            {
                Console.WriteLine("Invalid interval. Please enter a number between 1 and 86400.");
                return;
            }
            interval *= 1000; // convert to ms 
            string command = $"CONTINUOUS,{dateCode},{interval}";
            serial.Send(command);
        }

        private static void HandleReadCommand(SerialManager serial)
        {       
            var fileList = GetCsvFileList(serial);

            if (fileList.Count == 0)
            {
                Console.WriteLine("No CSV files found.");
                return;
            }

            Console.WriteLine("Select a file to read:");
            for (int i = 0; i < fileList.Count; i++)
            {
                Console.WriteLine($" {i + 1}: {fileList[i]}");
            }

            Console.Write("> ");
            if (!int.TryParse(Console.ReadLine(), out int choice) || choice < 1 || choice > fileList.Count)
            {
                Console.WriteLine("Invalid selection.");
                return;
            }

            string selectedFile = fileList[choice - 1];
            int maxLines = 15;
            string readCommand = $"READ,{selectedFile},{maxLines}";

            Console.WriteLine($"Reading last {maxLines} lines of {selectedFile}...");
            serial.Send(readCommand);

            while (true)
            {
                string line = serial.ReadLine()?.Trim();
                if (string.IsNullOrEmpty(line)) continue;

                if (line == "END") break;

                Console.WriteLine("[ARDUINO] " + line);
            }
        }

        static List<string> GetCsvFileList(SerialManager serial)
        {
            serial.Send("LIST");

            var fileList = new List<string>();

            while (true)
            {
                string line = serial.ReadLine()?.Trim();
                if (string.IsNullOrEmpty(line)) continue;

                if (line == "NO_FILES")
                {
                    Console.WriteLine("No CSV files found.");
                    return new List<string>();
                }

                if (line == "END_LIST")
                {
                    break;
                }

                fileList.Add(line);
            }

            return fileList;
        }

    }
}
