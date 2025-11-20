#include <stdio.h>
#include <stdlib.h>
#include "../include/neuroix.h"

int main() {
    printf("======================================\n");
    printf("      NEUROIX KERNEL - PRE-ALPHA      \n");
    printf("======================================\n\n");

    // 1. Init System
    nx_system_init();

    // 2. Config Setup
    NxConfig config = {
        .context_size = 2048,
        .n_threads = 4,
        .use_gpu = false
    };

    // 3. Create Process (Load Model)
    // Kita arahkan ke file path palsu dulu
    NxProcess* ai_proc = nx_create_process("models/tinyllama-1.1b-chat-v1.0.Q4_0.gguf", &config);

    if (!ai_proc) {
        fprintf(stderr, "FATAL: Failed to create process.\n");
        return 1;
    }

    // 4. Check Status
    nx_print_status(ai_proc);

    // 5. Interaction Loop (Simulasi 1 kali chat)
    printf("\n[USER] > Halo, apa kabar?\n");
    
    char* response = nx_send_command(ai_proc, "Halo, apa kabar?");
    printf("[AI]   > %s\n\n", response);
    
    // Penting: Free memory dari respon string (karena strdup di kernel)
    free(response);

    // 6. Cleanup
    nx_destroy_process(ai_proc);

    printf("System Shutdown.\n");
    return 0;
}
