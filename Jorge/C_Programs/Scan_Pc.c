#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <string.h>
#include <shlobj.h> // Para SHBrowseForFolder

void ShowResultDialog(const char* changes);

// Permite seleccionar una carpeta/unidad y la guarda en outPath
int SelectFolder(char* outPath, size_t outPathLen) {
    BROWSEINFO bi = { 0 };
    bi.lpszTitle = "Seleccione la carpeta o unidad a escanear";
    LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
    if (pidl != NULL) {
        SHGetPathFromIDList(pidl, outPath);
        CoTaskMemFree(pidl);
        return 1;
    }
    return 0;
}

void Scan() {
    // 1. Seleccionar carpeta/unidad
    char scanPath[MAX_PATH] = "C:\\";
    if (!SelectFolder(scanPath, sizeof(scanPath))) {
        MessageBoxA(NULL, "No se seleccionó ninguna carpeta. Escaneo cancelado.", "Matcom Guard", MB_OK | MB_ICONWARNING);
        return;
    }

    // 2. Crear snapshot.csv solo si no existe
    printf("Verificando existencia de snapshot.csv...\n");
    FILE* f = fopen("snapshot.csv", "r");
    if (!f) {
        printf("Creando snapshot.csv (primer escaneo)...\n");
        char cmd[1024];
        snprintf(cmd, sizeof(cmd),
            "powershell -Command \"Get-ChildItem -Path '%s' -Recurse -Force -ErrorAction SilentlyContinue | "
            "Select-Object FullName,Length,LastWriteTime | Export-Csv -Path snapshot.csv -NoTypeInformation -Encoding UTF8\"",
            scanPath);
        system(cmd);
        printf("Primer escaneo realizado (snapshot.csv).\n");
    } else {
        printf("snapshot.csv ya existe, no se modifica.\n");
        fclose(f);
    }

    // 3. Crear snapshot2.csv siempre (sobrescribe)
    printf("Creando snapshot2.csv (segundo escaneo)...\n");
    char cmd2[1024];
    snprintf(cmd2, sizeof(cmd2),
        "powershell -Command \"Get-ChildItem -Path '%s' -Recurse -Force -ErrorAction SilentlyContinue | "
        "Select-Object FullName,Length,LastWriteTime | Export-Csv -Path snapshot2.csv -NoTypeInformation -Encoding UTF8\"",
        scanPath);
    system(cmd2);
    printf("Segundo escaneo realizado (snapshot2.csv).\n");

    // 4. Comparar y mostrar en consola los cambios, guardar en malware_alert.txt (reiniciar archivo cada vez)
    printf("Comparando archivos (agregados, eliminados y modificados)...\n");
    // Reinicia malware_alert.txt antes de escribir
    FILE* malf = fopen("malware_alert.txt", "w");
    if (malf) fclose(malf);
    system(
        "powershell -Command \""
        "$a = Import-Csv snapshot.csv; "
        "$b = Import-Csv snapshot2.csv; "
        "$aMap = @{}; $a | ForEach-Object { $aMap[$_.FullName] = $_ }; "
        "$bMap = @{}; $b | ForEach-Object { $bMap[$_.FullName] = $_ }; "
        "$result = @(); "
        "foreach ($item in $b) { "
        "  if (-not $aMap.ContainsKey($item.FullName)) { "
        "    $result += 'AGREGADO: ' + $item.FullName "
        "  } elseif ($item.Length -ne $aMap[$item.FullName].Length -or $item.LastWriteTime -ne $aMap[$item.FullName].LastWriteTime) { "
        "    $result += 'MODIFICADO: ' + $item.FullName "
        "  } "
        "} "
        "foreach ($item in $a) { "
        "  if (-not $bMap.ContainsKey($item.FullName)) { "
        "    $result += 'ELIMINADO: ' + $item.FullName "
        "  } "
        "} "
        "$result | Out-File -Encoding UTF8 malware_alert.txt; "
        "$result | ForEach-Object { Write-Host $_ }"
        "\""
    );
    printf("Comparación finalizada.\n");

    // 5. Leer malware_alert.txt y mostrar en MessageBox con dos botones
    FILE* alert = fopen("malware_alert.txt", "r");
    if (alert) {
        fseek(alert, 0, SEEK_END);
        long len = ftell(alert);
        fseek(alert, 0, SEEK_SET);
        char* buffer = (char*)malloc(len + 1);
        if (buffer) {
            fread(buffer, 1, len, alert);
            buffer[len] = 0;
            // Mostrar resultados en consola en vez de MessageBox
            printf("\n===== RESULTADOS DEL ESCANEO =====\n%s\n", buffer);
            printf("\n¿Desea actualizar el snapshot base con el nuevo escaneo? (s/n): ");
            char resp[8] = {0};
            fgets(resp, sizeof(resp), stdin);
            if (resp[0] == 's' || resp[0] == 'S') {
                CopyFileA("snapshot2.csv", "snapshot.csv", FALSE);
                printf("Snapshot actualizado correctamente.\n");
            } else {
                printf("El snapshot base se mantiene sin cambios.\n");
            }
            free(buffer);
        }
        fclose(alert);
    }
    // Espera a que el usuario pulse Enter antes de cerrar la consola
    printf("\nPresione Enter para salir...");
    getchar();
}