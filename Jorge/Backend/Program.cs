using System;
using System.Diagnostics;
using System.IO;
using System.Net;
using System.Text.Json;
using System.Collections.Generic;
using System.Threading;

class Program
{
    static Process mainProc = null;

    static void Main(string[] args)
    {
        string exeFile = @"c:\D\School Projects\Matcom Guard\Jorge\C_Programs\main.exe";
        string scanExe = @"c:\D\School Projects\Matcom Guard\Jorge\C_Programs\Scan_Pc.exe";
        string scanC = @"c:\D\School Projects\Matcom Guard\Jorge\C_Programs\Scan_Pc.c";

        // Buscar y terminar cualquier proceso main.exe existente
        var running = Process.GetProcessesByName(Path.GetFileNameWithoutExtension("main.exe"));
        foreach (var proc in running)
        {
            try
            {
                proc.Kill();
                proc.WaitForExit();
            }
            catch { }
        }
        
        // Servidor HTTP para escaneo manual
        HttpListener listener = new HttpListener();
        listener.Prefixes.Add("http://localhost:5000/api/scan/");
        listener.Start();
        Console.WriteLine("Backend escuchando en http://localhost:5000/api/scan/");

        // Servidor HTTP para eventos
        HttpListener listenerEventos = new HttpListener();
        listenerEventos.Prefixes.Add("http://localhost:5000/api/events/");
        listenerEventos.Start();
        Console.WriteLine("Backend escuchando en http://localhost:5000/api/events/");

        // Servidor HTTP para monitoreo
        HttpListener listenerMonitor = new HttpListener();
        listenerMonitor.Prefixes.Add("http://localhost:5000/api/monitor/");
        listenerMonitor.Start();
        Console.WriteLine("Backend escuchando en http://localhost:5000/api/monitor/");

        // Lanzar cada listener en un hilo separado
        new Thread(() =>
        {
            while (true)
            {
                try
                {
                    var contextMonitor = listenerMonitor.GetContext();
                    var req = contextMonitor.Request;
                    var resp = contextMonitor.Response;
                    resp.AddHeader("Access-Control-Allow-Origin", "*");
                    resp.ContentType = "application/json";
                    if (req.HttpMethod == "POST")
                    {
                        // Compilar main.c si no existe main.exe
                        string mainC = @"c:\D\School Projects\Matcom Guard\Jorge\C_Programs\main.c";
                        string mainExe = @"c:\D\School Projects\Matcom Guard\Jorge\C_Programs\main.exe";
                        if (!File.Exists(mainExe))
                        {
                            var compileProc = new Process();
                            compileProc.StartInfo.FileName = "cmd.exe";
                            compileProc.StartInfo.Arguments = $"/C gcc \"{mainC}\" -o \"{mainExe}\" -lws2_32";
                            compileProc.StartInfo.CreateNoWindow = true;
                            compileProc.StartInfo.UseShellExecute = false;
                            compileProc.Start();
                            compileProc.WaitForExit();
                        }
                        // Si ya está corriendo, terminarlo
                        var runningProcs = Process.GetProcessesByName(Path.GetFileNameWithoutExtension("main.exe"));
                        foreach (var proc in runningProcs)
                        {
                            try { proc.Kill(); proc.WaitForExit(); } catch { }
                        }
                        // Lanzar main.exe en nueva consola
                        mainProc = new Process();
                        mainProc.StartInfo.FileName = mainExe;
                        mainProc.StartInfo.UseShellExecute = true;
                        mainProc.Start();
                        var monitorJson = JsonSerializer.Serialize(new { running = true });
                        using (var writer = new StreamWriter(resp.OutputStream))
                            writer.Write(monitorJson);
                    }
                    else // GET
                    {
                        bool isRunning = false;
                        var procs = Process.GetProcessesByName(Path.GetFileNameWithoutExtension("main.exe"));
                        if (procs.Length > 0) isRunning = true;
                        var monitorJson = JsonSerializer.Serialize(new { running = isRunning });
                        using (var writer = new StreamWriter(resp.OutputStream))
                            writer.Write(monitorJson);
                    }
                    resp.Close();
                }
                catch (Exception) { /* Ignora errores de conexión cerrada */ }
            }
        }).Start();

        // El resto de listeners igual, pero sin el bloque de monitoreo:
        while (true)
        {
            // Manejo de escaneo
            try
            {
                var context = listener.GetContext();
                var response = context.Response;
                string result = "";

                // Compila Scan_Pc.c si no existe el exe
                if (!File.Exists(scanExe))
                {
                    var compileProc = new Process();
                    compileProc.StartInfo.FileName = "cmd.exe";
                    compileProc.StartInfo.Arguments = $"/C gcc \"{scanC}\" -o \"{scanExe}\"";
                    compileProc.StartInfo.CreateNoWindow = true;
                    compileProc.StartInfo.UseShellExecute = false;
                    compileProc.Start();
                    compileProc.WaitForExit();
                }

                // Ejecuta el escaneo en una nueva terminal
                try
                {
                    // Lanza Scan_Pc.exe en una nueva ventana de consola
                    var scanProc = new Process();
                    scanProc.StartInfo.FileName = "cmd.exe";
                    scanProc.StartInfo.Arguments = $"/C start \"Escaneo Profundo\" \"{scanExe}\"";
                    scanProc.StartInfo.UseShellExecute = true;
                    scanProc.Start();
                    result = "Escaneo profundo iniciado en una nueva ventana.";
                }
                catch (Exception ex)
                {
                    result = "Error ejecutando escaneo: " + ex.Message;
                }

                response.AddHeader("Access-Control-Allow-Origin", "*");
                using (var writer = new StreamWriter(response.OutputStream))
                {
                    writer.Write(result);
                }
                response.Close();
            }
            catch (Exception) { /* Ignora errores de conexión cerrada */ }

            // Manejo de eventos
            try
            {
                var contextEventos = listenerEventos.GetContext();
                var responseEventos = contextEventos.Response;
                responseEventos.AddHeader("Access-Control-Allow-Origin", "*");

                List<string> eventos = new List<string>();
                string logFile = @"c:\D\School Projects\Matcom Guard\Jorge\cambio_sospechoso.log";
                if (File.Exists(logFile))
                {
                    eventos.AddRange(File.ReadAllLines(logFile));
                }

                string eventosJson = JsonSerializer.Serialize(eventos);
                using (var writer = new StreamWriter(responseEventos.OutputStream))
                {
                    writer.Write(eventosJson);
                }
                responseEventos.Close();
            }
            catch (Exception) { /* Ignora errores de conexión cerrada */ }
        }
    }
}