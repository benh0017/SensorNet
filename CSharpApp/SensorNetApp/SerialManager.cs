using System;
using System.IO.Ports;
using System.Text;
using System.Threading;

namespace SensorNetApp
{
    public class SerialManager
    {
        private SerialPort _port;
        private StringBuilder _buffer = new StringBuilder();

        public bool Connect(string portName, int baudRate = 9600)
        {
            try
            {
                _port = new SerialPort(portName, baudRate)
                {
                    ReadTimeout = 500,
                    WriteTimeout = 500
                };

                _port.Open();
                Console.WriteLine($"Connected to {portName}.");
                return true;
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Failed to open {portName}: {ex.Message}");
                return false;
            }
        }

        public void Send(string data)
        {
            if (_port != null && _port.IsOpen)
            {
                try
                {
                    _port.WriteLine(data);
                }
                catch (Exception ex)
                {
                    Console.WriteLine($"Send error: {ex.Message}");
                }
            }
        }

        public string ReadLine()
        {
            if (_port != null && _port.IsOpen && _port.BytesToRead > 0)
            {
                try
                {
                    return _port.ReadLine();
                }
                catch (TimeoutException) { }
                catch (Exception ex)
                {
                    Console.WriteLine($"Read error: {ex.Message}");
                }
            }

            return string.Empty;
        }

        public void Disconnect()
        {
            if (_port != null && _port.IsOpen)
            {
                _port.Close();
                Console.WriteLine("Disconnected.");
            }
        }
    }
}
