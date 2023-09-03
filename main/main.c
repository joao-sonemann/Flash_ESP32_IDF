/* Finding Partitions Example
   This example code is in the Public Domain (or CC0 licensed, at your option.)
   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <string.h>
#include <assert.h>
#include "esp_partition.h"
#include "esp_log.h"
#include "esp_err.h"
#include "cJSON.h"
//#include "sCommandList.h"

#define SSID_MAX_LEN 32

static const char *TAG = "example";

//typedef struct{ entra no lugar do que seria a label
//    char uuid[UUID_LENGHT]; // identificador do comando unico
//    uint8_t mode; // Um dos defines
//    uint8_t pin_num;
//    uint64_t period_us;
//    void* cmd_struct;
//    sControl_t* control;
//} sCommand_t;
typedef struct{
    char palavra1[SSID_MAX_LEN];
} label1_t;

void clean_string(char *str) {
    char *cleaned = malloc(strlen(str) + 1); // allocate a buffer to hold the cleaned string
    if (!cleaned) {
        // error handling if malloc fails
        return;
    }
    
    int i = 0, j = 0;
    while (str[i] != '\0') {
        if (str[i] < 128) { // check if the character is an ASCII character
            cleaned[j++] = str[i];
        }
        i++;
    }
    cleaned[j] = '\0'; // terminate the cleaned string
    
    strcpy(str, cleaned); // copy the cleaned string back to the original string
    free(cleaned); // free the temporary buffer
}


esp_err_t Write_Data_Flash(label1_t data, char* label){
    // achar partition do pin e escrever nela

    const esp_partition_t *partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, label);
    assert(partition != NULL);
    size_t data_len = sizeof(label1_t);

    // Prepare data to be read later using the mapped address
    ESP_ERROR_CHECK(esp_partition_erase_range(partition, 0, partition->size));
    ESP_ERROR_CHECK(esp_partition_write(partition, 0, &data, data_len));
    
    return ESP_OK;
}

esp_err_t Read_Data_Flash(label1_t* data, char* label){
    //passar numero do pino achar parition e preencher dentro do data

    const esp_partition_t *partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, label);
    assert(partition != NULL);
    const void *map_ptr;
    spi_flash_mmap_handle_t map_handle;

    // Map the partition to data memory
    ESP_ERROR_CHECK(esp_partition_mmap(partition, 0, partition->size, SPI_FLASH_MMAP_DATA, &map_ptr, &map_handle));
    ESP_LOGI(TAG, "Mapped partition to data memory address %p", map_ptr);


    // Read back the written verification data using the mapped memory pointer
    memcpy(data, map_ptr, sizeof(label1_t));
    clean_string(data->palavra1);
   
    
    spi_flash_munmap(map_handle);
    ESP_LOGI(TAG, "Unmapped partition from data memory");

    return ESP_OK;
}

void map_partition(){
    
    esp_partition_iterator_t iterator = esp_partition_find(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, NULL);

    if (iterator == NULL) {
        //caso em que nenhuma partição foi encontrada
        return;
    }

    // Percorre todas as partições encontradas usando o iterador
    while (iterator != NULL) {

            const esp_partition_t *part = esp_partition_get(iterator);

            char* nome_label1;
            nome_label1 = (char*) malloc(sizeof(char) * (strlen("label1") + 1));
            strcpy(nome_label1, "label1");
            ESP_LOGI(TAG, "nome_label: %s", nome_label1);

            if(strcmp(nome_label1, part->label) == 0){

                char label[] = "label1";
                //static const char SSID[SSID_MAX_LEN] = "MY_SSID_EXAMPLE";
                //LABEL1
                const label1_t label1 = {
                    .palavra1 = "label1doteste",
                };
                label1_t mem_label1 = {
                    .palavra1 = "",
                };

                ESP_ERROR_CHECK(Write_Data_Flash(label1, label));
                ESP_LOGI(TAG, "Written sample data to partition: %s", label1.palavra1);
            
                //char memSSID[SSID_MAX_LEN];
                
                Read_Data_Flash(&mem_label1, label);
                ESP_LOGI(TAG, "Read sample data from partition using mapped memory: %s", (char*) mem_label1.palavra1);
                
                if(strcmp(label1.palavra1, mem_label1.palavra1) == 0)
                    ESP_LOGI(TAG, "Data matches");
                else
                    ESP_LOGI(TAG, "Data dont't matches");
            } 


        printf("Partition name: %s, Type: %d, Subtype: %d\n", part->label, part->type, part->subtype);
        iterator = esp_partition_next(iterator);
    }
    esp_partition_iterator_release(iterator);
}

void app_main(void)
{
    /*
    * This example uses the partition table from ../partitions_example.csv. For reference, its contents are as follows:
    *
        nvs,            data,   nvs,        0x9000,     0x6000,
        phy_init,       data,   phy,        0xf000,     0x1000,
        factory,        app,    factory,    0x10000,    1M,
        label1,         data,   ,           ,           0x01000,
        label2,         data,   ,           ,           0x01000,
        label3,         data,   ,           ,           0x01000,
    */
    // Find the partition map in the partition table
    // Unmap mapped memory
    //spi_flash_munmap(map_handle);

    map_partition();

    ESP_LOGI(TAG, "Example end");
}