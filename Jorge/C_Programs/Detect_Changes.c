#include <stdio.h>
#include <windows.h>

void Detect_Changes()
{
    static ULONGLONG last_free = 0;
    ULONGLONG current_free = Scan_Files_Directory();
    if (last_free == 0)
    {
        last_free = current_free;
        return;
    }
    // Umbral de 10 MB para considerar un cambio sospechoso
    ULONGLONG threshold = 10 * 1024 * 1024;
    LONGLONG diff = (LONGLONG)current_free - (LONGLONG)last_free;
    if (llabs(diff) > (LONGLONG)threshold)
    {
        printf("cambio sospechoso: %lld bytes\n", diff);
        FILE *f = fopen("cambio_sospechoso.log", "a");
        if (f)
        {
            time_t now = time(NULL);
            char *timestr = ctime(&now);
            if (timestr)
                timestr[strcspn(timestr, "\n")] = 0; // Remove newline
            fprintf(f, "[%s] cambio sospechoso: %lld bytes\n", timestr ? timestr : "?", diff);
            fclose(f);
        }
        last_free = current_free;
    }
}
