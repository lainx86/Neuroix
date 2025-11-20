#ifndef NEUROIX_H
#define NEUROIX_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// ==========================================
// 1. SYSTEM CONSTANTS
// ==========================================
#define NX_MAX_PATH 256
#define NX_DEFAULT_CTX_SIZE 2048  // Default RAM "Context Window" (2k tokens)
#define NX_DEFAULT_THREADS 4      // Jumlah CPU threads yang dipakai

// ==========================================
// 2. STATE DEFINITIONS
// ==========================================
// Status proses AI saat ini (State Machine)
typedef enum {
    NX_STATE_UNINITIALIZED = 0,
    NX_STATE_READY,     // Model di RAM, siap menerima input
    NX_STATE_THINKING,  // Sedang melakukan inferensi (CPU/GPU sibuk)
    NX_STATE_ERROR      // Terjadi kesalahan (OOM, file corrupt)
} NxState;

// ==========================================
// 3. DATA STRUCTURES (The PCB)
// ==========================================

// Konfigurasi saat meluncurkan proses baru
typedef struct {
    uint32_t context_size;  // Berapa banyak token yang diingat
    uint8_t n_threads;      // Alokasi CPU core
    bool use_gpu;           // Coba offload ke GPU jika ada
} NxConfig;

// Neuroix Process Control Block (PCB)
// Ini adalah representasi satu "Otak" di memori
typedef struct {
    int pid;                    // Process ID
    char name[64];              // Nama proses (misal: "assistant")
    char model_path[NX_MAX_PATH];
    
    // Opaque Pointers (Pointer ke objek C++ llama.cpp)
    // Kita pakai void* agar header ini tetap valid sebagai C murni
    void* backend_model;        // Pointer ke Weights (Read-only)
    void* backend_context;      // Pointer ke KV Cache (Read-write RAM)
    
    NxState state;              // Status saat ini
    size_t ram_usage_bytes;     // Telemetri penggunaan memori
    
    NxConfig config;            // Config yang dipakai saat launch
} NxProcess;

// ==========================================
// 4. KERNEL FUNCTION PROTOTYPES
// ==========================================

// --- System Level ---
void nx_system_init();    // Inisialisasi backend global

// --- Process Management (Lifecycle) ---
// Load model dari disk ke RAM (seperti 'exec' di Unix)
NxProcess* nx_create_process(const char* model_path, NxConfig* config);

// Hapus model dari RAM (seperti 'kill' di Unix)
void nx_destroy_process(NxProcess* proc);

// --- I/O & Inference ---
// Mengirim prompt dan mendapatkan jawaban
// Note: Di implementasi nyata, ini harusnya streaming/callback, 
// tapi untuk tahap awal kita buat blocking (tunggu sampai selesai).
char* nx_send_command(NxProcess* proc, const char* input_text);

// --- Telemetry ---
void nx_print_status(NxProcess* proc);

#endif // NEUROIX_H
