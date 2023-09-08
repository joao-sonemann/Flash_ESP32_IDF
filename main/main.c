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

#include "sCommandList.h"

#define SSID_MAX_LEN 32

static const char *TAG = "example";


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


esp_err_t Write_Data_Flash(sCommand_t data, char* label){
    // achar partition do pin e escrever nela

    const esp_partition_t *partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, label);
    assert(partition != NULL);
    size_t data_len = sizeof(sCommand_t);

    // Prepare data to be read later using the mapped address
    ESP_ERROR_CHECK(esp_partition_erase_range(partition, 0, partition->size));
    ESP_ERROR_CHECK(esp_partition_write(partition, 0, &data, data_len));
    
    return ESP_OK;
}

esp_err_t Read_Data_Flash(sCommand_t* data, char* label){
    //passar numero do pino achar parition e preencher dentro do data

    const esp_partition_t *partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, label);
    assert(partition != NULL);
    const void *map_ptr;
    spi_flash_mmap_handle_t map_handle;

    // Map the partition to data memory
    ESP_ERROR_CHECK(esp_partition_mmap(partition, 0, partition->size, SPI_FLASH_MMAP_DATA, &map_ptr, &map_handle));
    ESP_LOGI(TAG, "Mapped partition to data memory address %p", map_ptr);


    // Read back the written verification data using the mapped memory pointer
    memcpy(data, map_ptr, sizeof(sCommand_t));
    //clean_string(data->pin_num);
   
    
    spi_flash_munmap(map_handle);
    ESP_LOGI(TAG, "Unmapped partition from data memory");

    return ESP_OK;
}

void map_partition(sCommand_t dados){
    
   
    char numero_pino[10];
    int i;
    sprintf(numero_pino, "gpio%d", dados.pin_num);
    ESP_LOGI(TAG, "numero pino: %s", numero_pino);
   

    esp_partition_iterator_t iterator = esp_partition_find(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, NULL);

    if (iterator == NULL) {
        //caso em que nenhuma partição foi encontrada
        ESP_LOGI(TAG, "nenhuma partiçao encontrada");
        return;
    }

    // Percorre todas as partições encontradas usando o iterador
    for(i=0; i<=3; i++){
     ESP_LOGI(TAG, "part: %d", i);
            const esp_partition_t *part = esp_partition_get(iterator);
            if (iterator == NULL) {
                //caso em que nenhuma partição foi encontrada
                ESP_LOGI(TAG, "FIM PARTIÇOES");
                
            }
            if(strcmp(numero_pino, part->label) == 0){
                   printf("Encontrado\n");

                char label[] = "gpio1";


                const sCommand_t label_dados = {
                    .pin_num    = dados.pin_num,
                    .mode       = dados.mode,
                };
              
                sCommand_t mem_dados = {
                    .pin_num = "",
                    .mode    = "",
                };

                ESP_ERROR_CHECK(Write_Data_Flash(label_dados, label));
                ESP_LOGI(TAG, "Written sample pin to partition: %d", dados.pin_num);
                ESP_LOGI(TAG, "Written sample mode to partition: %d", dados.mode);
                
                Read_Data_Flash(&mem_dados, label);
                ESP_LOGI(TAG, "Read sample pin from partition using mapped memory: %d", mem_dados.pin_num);
                ESP_LOGI(TAG, "Read sample mode from partition using mapped memory: %d", mem_dados.mode);
                
                //if(strcmp(label, mem_dados.pin_num) == 0)
                //   ESP_LOGI(TAG, "Data matches");
                //else
                //    ESP_LOGI(TAG, "Data dont't matches");
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

    sCommand_t dados;
    dados.pin_num   =   1;
    dados.mode      =   42;

    map_partition(dados);

    ESP_LOGI(TAG, "Example end");
}