using System;
using System.Net;
using System.Text;
using System.Diagnostics;

class Program
{
    static void Main(string[] args)
    {
        HttpListener listener = new HttpListener();
        listener.Prefixes.Add("http://localhost:5000/");
        listener.Start();
        Console.WriteLine("Servidor HTTP escuchando en http://localhost:5000/");

        while (true)
        {
            HttpListenerContext context = listener.GetContext();
            HttpListenerRequest request = context.Request;
            HttpListenerResponse response = context.Response;

            // Habilita CORS
            response.AddHeader("Access-Control-Allow-Origin", "*");

            if (request.Url.AbsolutePath == "/main")
            {
                try
                {
                    ProcessStartInfo psi = new ProcessStartInfo();
                    psi.FileName = @"C:\D\School Projects\Matcom Guard\Matcom_Web_Guard\Jorge\C_Programs\main.exe"; // Ruta al ejecutable
                    psi.UseShellExecute = true; // Para mostrar la consola
                    Process.Start(psi);

                    string msg = "Proceso detect_devices iniciado.";
                    byte[] buffer = Encoding.UTF8.GetBytes(msg);
                    response.ContentLength64 = buffer.Length;
                    response.ContentType = "text/plain";
                    response.OutputStream.Write(buffer, 0, buffer.Length);
                }
                catch (Exception ex)
                {
                    string msg = "Error al iniciar detect_devices: " + ex.Message;
                    byte[] buffer = Encoding.UTF8.GetBytes(msg);
                    response.StatusCode = 500;
                    response.ContentLength64 = buffer.Length;
                    response.ContentType = "text/plain";
                    response.OutputStream.Write(buffer, 0, buffer.Length);
                }
            }
            else if (request.Url.AbsolutePath == "/scan")
            {
                try
                {
                    ProcessStartInfo psi = new ProcessStartInfo();
                    // Asegúrate de que la ruta aquí sea correcta y el archivo exista
                    psi.FileName = @"C:\D\School Projects\Matcom Guard\Matcom_Web_Guard\Jorge\C_Programs\Scan_Pc.exe";
                    psi.UseShellExecute = true;
                    Process.Start(psi);

                    string msg = "Proceso Scan_Pc iniciado.";
                    byte[] buffer = Encoding.UTF8.GetBytes(msg);
                    response.ContentLength64 = buffer.Length;
                    response.ContentType = "text/plain";
                    response.OutputStream.Write(buffer, 0, buffer.Length);
                }
                catch (Exception ex)
                {
                    string msg = "Error al iniciar Scan_Pc: " + ex.Message;
                    byte[] buffer = Encoding.UTF8.GetBytes(msg);
                    response.StatusCode = 500;
                    response.ContentLength64 = buffer.Length;
                    response.ContentType = "text/plain";
                    response.OutputStream.Write(buffer, 0, buffer.Length);
                }
            }
            else
            {
                string responseString = "Hola desde C#";
                byte[] buffer = Encoding.UTF8.GetBytes(responseString);
                response.ContentLength64 = buffer.Length;
                response.ContentType = "text/plain";
                response.OutputStream.Write(buffer, 0, buffer.Length);
            }
            response.OutputStream.Close();
        }
    }
}