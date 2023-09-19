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
    char *cleaned = malloc(strlen(str) + 1); 
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


     //Read back the written verification data using the mapped memory pointer
    memcpy(data, map_ptr, sizeof(sCommand_t));
    //clean_string(data->pin_num);
   
    
    spi_flash_munmap(map_handle);
    ESP_LOGI(TAG, "Unmapped partition from data memory");

    return ESP_OK;
}


esp_err_t Write_Data_Control(sControl_t control, char* label){
    // achar partition do pin e escrever nela

    const esp_partition_t *partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, label);
    assert(partition != NULL);
    size_t data_len = sizeof(sControl_t);

    // Prepare data to be read later using the mapped address
    ESP_ERROR_CHECK(esp_partition_erase_range(partition, 125, partition->size - (125 % 4096) ));
    ESP_ERROR_CHECK(esp_partition_write(partition, 125, &control, data_len));
    
    return ESP_OK;
}

esp_err_t Read_Data_Control(sControl_t* control, char* label){
    //passar numero do pino achar parition e preencher dentro do data

    const esp_partition_t *partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, label);
    assert(partition != NULL);
    const void *map_ptr;
    spi_flash_mmap_handle_t map_handle;

    // Map the partition to data memory
    ESP_ERROR_CHECK(esp_partition_mmap(partition, 125, partition->size - (125 % 4096), SPI_FLASH_MMAP_DATA, &map_ptr, &map_handle));
    ESP_LOGI(TAG, "Mapped partition to data memory address %p", map_ptr);


     //Read back the written verification data using the mapped memory pointer
    memcpy(control, map_ptr, sizeof(sControl_t));
    //clean_string(data->pin_num);
   
    
    spi_flash_munmap(map_handle);
    ESP_LOGI(TAG, "Unmapped partition from data memory");

    return ESP_OK;
}



void map_partition(sCommand_t dados, sControl_t control){

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
    for(i=0; i<=40; i++){
     ESP_LOGI(TAG, "part: %d", i);
            const esp_partition_t *part = esp_partition_get(iterator);
            if (iterator == NULL) {
                //caso em que nenhuma partição foi encontrada
                ESP_LOGI(TAG, "FIM PARTIÇOES");
                
            }
            if(strcmp(numero_pino, part->label) == 0){
                   printf("Encontrado\n");

                char label[] = "gpio1";
                    //sem esta label nao esta funcionando, mas com ela assim consigo fazer operaçoes em todas as partiçoes

                const sCommand_t label_dados = {
                    .uuid               = "",
                    .mode               = dados.mode,
                    .pin_num            = dados.pin_num,
                    .period_us          = dados.period_us,
                };
                    //.cmd_struct = dados.cmd_struct,
                    //control,

                strcpy(label_dados.uuid, dados.uuid);

                sCommand_t mem_dados = {
                    .uuid               = "",
                    .mode               = "",
                    .pin_num            = "",
                    .period_us          = "",
                };

                ESP_ERROR_CHECK(Write_Data_Flash(label_dados, label));
                
                ESP_LOGI(TAG, "Written uuid to partition: %s", dados.uuid);
                ESP_LOGI(TAG, "Written mode to partition: %d", dados.mode);
                ESP_LOGI(TAG, "Written pin to partition: %d", dados.pin_num);
                ESP_LOGI(TAG, "Written period to partition: %llu", dados.period_us);

                
                Read_Data_Flash(&mem_dados, label);

                ESP_LOGI(TAG, "Read uuid from partition using mapped memory: %s", mem_dados.uuid);
                ESP_LOGI(TAG, "Read mode from partition using mapped memory: %d", mem_dados.mode);
                ESP_LOGI(TAG, "Read pin from partition using mapped memory: %d", mem_dados.pin_num);
                ESP_LOGI(TAG, "Read period from partition using mapped memory: %llu", mem_dados.period_us);

                
                const sControl_t label_control = {
                    .actuator_pin_num    = control.actuator_pin_num,
                    .mode               = control.mode,
                };

                sControl_t mem_control = {
                    .actuator_pin_num     ="",
                    .mode                ="",
                };

                ESP_ERROR_CHECK(Write_Data_Control(label_control, label));
                
                //ESP_LOGI(TAG, "Written uuid to partition: %s", dados.uuid);
                //ESP_LOGI(TAG, "Written mode to partition: %d", dados.mode);
                //ESP_LOGI(TAG, "Written pin to partition: %d", dados.pin_num);
                //ESP_LOGI(TAG, "Written period to partition: %llu", dados.period_us);
                
                Read_Data_Control(&mem_control, label);

                //ESP_LOGI(TAG, "Read uuid from partition using mapped memory: %s", mem_dados.uuid);
                //ESP_LOGI(TAG, "Read mode from partition using mapped memory: %d", mem_dados.mode);
                //ESP_LOGI(TAG, "Read pin from partition using mapped memory: %d", mem_dados.pin_num);
                //ESP_LOGI(TAG, "Read period from partition using mapped memory: %llu", mem_dados.period_us);


                
             }


        printf("Partition name: %s, Type: %d, Subtype: %d\n", part->label, part->type, part->subtype);
        iterator = esp_partition_next(iterator);
    }


    esp_partition_iterator_release(iterator);
}

void app_main(void)
{
    sCommand_t dados;
    sControl_t control;

    dados.pin_num = 2;
    strcpy(dados.uuid, "casas");
    
    map_partition(dados, control);
    ESP_LOGI(TAG, "Example end");
}