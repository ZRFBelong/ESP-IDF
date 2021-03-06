// Copyright 2017-2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef _ESP_BLE_MESH_NETWORKING_API_H_
#define _ESP_BLE_MESH_NETWORKING_API_H_

#include "esp_ble_mesh_defs.h"

/** @brief: event, event code of user-defined model events; param, parameters of user-defined model events */
typedef void (* esp_ble_mesh_model_cb_t)(esp_ble_mesh_model_cb_event_t event,
        esp_ble_mesh_model_cb_param_t *param);

/**
 * @brief         Register BLE Mesh callback for user-defined models' operations.
 *                This callback can report the following events generated for the user-defined models:
 *                - Call back the messages received by user-defined client and server models to the
 *                  application layer;
 *                - If users call esp_ble_mesh_server/client_model_send, this callback notifies the
 *                  application layer of the send_complete event;
 *                - If user-defined client model sends a message that requires response, and the response
 *                  message is received after the timer expires, the response message will be reported
 *                  to the application layer as published by a peer device;
 *                - If the user-defined client model fails to receive the response message during a specified
 *                  period of time, a timeout event will be reported to the application layer.
 *
 * @note          The client models (i.e. Config Client model, Health Client model, Generic
 *                Client models, Sensor Client model, Scene Client model and Lighting Client models)
 *                that have been realized internally have their specific register functions.
 *                For example, esp_ble_mesh_register_config_client_callback is the register
 *                function for Config Client Model.
 *
 * @param[in]     callback: Pointer to the callback function.
 *
 * @return        ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_register_custom_model_callback(esp_ble_mesh_model_cb_t callback);

/**
 * @brief        Add the message opcode to the beginning of the model message
 *               before sending or publishing the model message.
 *
 * @note         This API is only used to set the opcode of the message.
 *
 * @param[in]    data: Pointer to the message data.
 * @param[in]    opcode: The message opcode.
 *
 * @return       ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_model_msg_opcode_init(uint8_t *data, uint32_t opcode);

/**
 * @brief         Initialize the user-defined client model. All user-defined client models
 *                shall call this function to initialize the client model internal data.
 *                Node: Before calling this API, the op_pair_size and op_pair variabled within
 *                      the user_data(defined using esp_ble_mesh_client_t_) of the client model
 *                      need to be initialized.
 *
 * @param[in]     model: BLE Mesh Client model to which the message belongs.
 *
 * @return        ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_client_model_init(esp_ble_mesh_model_t *model);

/**
 * @brief         De-initialize the user-defined client model.
 *
 * @note          This function shall be invoked before esp_ble_mesh_deinit() is called.
 *
 * @param[in]     model: Pointer of the Client model.
 *
 * @return        ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_client_model_deinit(esp_ble_mesh_model_t *model);

/**
 * @brief         Send server model messages(such as server model status messages).
 *
 * @param[in]     model: BLE Mesh Server Model to which the message belongs.
 * @param[in]     ctx:   Message context, includes keys, TTL, etc.
 * @param[in]     opcode: Message opcode.
 * @param[in]     length: Message length (exclude the message opcode).
 * @param[in]     data: Parameters of Access Payload (exclude the message opcode) to be sent.
 *
 * @return        ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_server_model_send_msg(esp_ble_mesh_model_t *model,
        esp_ble_mesh_msg_ctx_t *ctx, uint32_t opcode,
        uint16_t length, uint8_t *data);

/**
 * @brief         Send client model message (such as model get, set, etc).
 *
 * @param[in]     model: BLE Mesh Client Model to which the message belongs.
 * @param[in]     ctx:   Message context, includes keys, TTL, etc.
 * @param[in]     opcode: Message opcode.
 * @param[in]     length: Message length (exclude the message opcode).
 * @param[in]     data: Parameters of the Access Payload (exclude the message opcode) to be sent.
 * @param[in]     msg_timeout: Time to get response to the message (in milliseconds).
 * @param[in]     need_rsp: TRUE if the opcode requires the peer device to reply, FALSE otherwise.
 * @param[in]     device_role: Role of the device (Node/Provisioner) that sends the message.
 *
 * @return        ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_client_model_send_msg(esp_ble_mesh_model_t *model,
        esp_ble_mesh_msg_ctx_t *ctx, uint32_t opcode,
        uint16_t length, uint8_t *data, int32_t msg_timeout,
        bool need_rsp, esp_ble_mesh_dev_role_t device_role);

/**
 * @brief         Send a model publication message.
 *
 * @note          Before calling this function, the user needs to ensure that the model
 *                publication message (@ref esp_ble_mesh_model_pub_t.msg) contains a valid
 *                message to be sent. And if users want to update the publishing message,
 *                this API should be called in ESP_BLE_MESH_MODEL_PUBLISH_UPDATE_EVT
 *                with the message updated.
 *
 *
 * @param[in]    model: Mesh (client) Model publishing the message.
 * @param[in]    opcode: Message opcode.
 * @param[in]    length: Message length (exclude the message opcode).
 * @param[in]    data: Parameters of the Access Payload (exclude the message opcode) to be sent.
 * @param[in]    device_role: Role of the device (node/provisioner) publishing the message of the type esp_ble_mesh_dev_role_t.
 *
 * @return       ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_model_publish(esp_ble_mesh_model_t *model, uint32_t opcode,
                                     uint16_t length, uint8_t *data,
                                     esp_ble_mesh_dev_role_t device_role);

/**
 * @brief        Update a server model state value. If the model publication
 *               state is set properly (e.g. publish address is set to a valid
 *               address), it will publish corresponding status message.
 *
 * @note         Currently this API is used to update bound state value, not
 *               for all server model states.
 *
 * @param[in]    model: Server model which is going to update the state.
 * @param[in]    type:  Server model state type.
 * @param[in]    value: Server model state value.
 *
 * @return       ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_server_model_update_state(esp_ble_mesh_model_t *model,
        esp_ble_mesh_server_state_type_t type,
        esp_ble_mesh_server_state_value_t *value);

/**
 * @brief         Reset the provisioning procedure of the local BLE Mesh node.
 *
 * @note          All provisioning information in this node will be deleted and the node
 *                needs to be reprovisioned. The API function esp_ble_mesh_node_prov_enable()
 *                needs to be called to start a new provisioning procedure.
 *
 * @return        ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_node_local_reset(void);

/**
 * @brief        This function is called to set the node (provisioned device) name.
 *
 * @param[in]    index: Index of the node in the node queue.
 * @param[in]    name: Name (end by '\0') to be set for the node.
 *
 * @note         index is obtained from the parameters of ESP_BLE_MESH_PROVISIONER_PROV_COMPLETE_EVT.
 *
 * @return       ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_provisioner_set_node_name(uint16_t index, const char *name);

/**
 * @brief        This function is called to get the node (provisioned device) name.
 *
 * @param[in]    index: Index of the node in the node queue.
 *
 * @note         index is obtained from the parameters of ESP_BLE_MESH_PROVISIONER_PROV_COMPLETE_EVT.
 *
 * @return       Node name on success, or NULL on failure.
 *
 */
const char *esp_ble_mesh_provisioner_get_node_name(uint16_t index);

/**
 * @brief        This function is called to get the node (provisioned device) index.
 *
 * @param[in]    name: Name of the node (end by '\0').
 *
 * @return       Node index on success, or an invalid value (0xFFFF) on failure.
 *
 */
uint16_t esp_ble_mesh_provisioner_get_node_index(const char *name);

/**
 * @brief        This function is called to store the Composition Data of the node.
 *
 * @param[in]    unicast_addr: Element address of the node
 * @param[in]    data:         Pointer of Composition Data
 * @param[in]    length:       Length of Composition Data
 *
 * @return       ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_provisioner_store_node_comp_data(uint16_t unicast_addr, uint8_t *data, uint16_t length);

/**
 * @brief        This function is called to get the provisioned node information
 *               with the node device uuid.
 *
 * @param[in]    uuid: Device UUID of the node
 *
 * @return       Pointer of the node info struct or NULL on failure.
 *
 */
esp_ble_mesh_node_t *esp_ble_mesh_provisioner_get_node_with_uuid(const uint8_t uuid[16]);

/**
 * @brief        This function is called to get the provisioned node information
 *               with the node unicast address.
 *
 * @param[in]    unicast_addr: Unicast address of the node
 *
 * @return       Pointer of the node info struct or NULL on failure.
 *
 */
esp_ble_mesh_node_t *esp_ble_mesh_provisioner_get_node_with_addr(uint16_t unicast_addr);

/**
 * @brief        This function is called to delete the provisioned node information
 *               with the node device uuid.
 *
 * @param[in]    uuid: Device UUID of the node
 *
 * @return       ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_provisioner_delete_node_with_uuid(const uint8_t uuid[16]);

/**
 * @brief        This function is called to delete the provisioned node information
 *               with the node unicast address.
 *
 * @param[in]    unicast_addr: Unicast address of the node
 *
 * @return       ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_provisioner_delete_node_with_addr(uint16_t unicast_addr);

/**
 * @brief         This function is called to add a local AppKey for Provisioner.
 *
 * @param[in]     app_key: The app key to be set for the local BLE Mesh stack.
 * @param[in]     net_idx: The network key index.
 * @param[in]     app_idx: The app key index.
 *
 * @note          app_key: If set to NULL, app_key will be generated internally.
 *                net_idx: Should be an existing one.
 *                app_idx: If it is going to be generated internally, it should be set to
 *                         0xFFFF, and the new app_idx will be reported via an event.
 *
 * @return        ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_provisioner_add_local_app_key(const uint8_t app_key[16], uint16_t net_idx, uint16_t app_idx);

/**
 * @brief         This function is used to update a local AppKey for Provisioner.
 *
 * @param[in]     app_key: Value of the AppKey.
 * @param[in]     net_idx: Corresponding NetKey Index.
 * @param[in]     app_idx: The AppKey Index
 *
 * @return        ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_provisioner_update_local_app_key(const uint8_t app_key[16],
                uint16_t net_idx, uint16_t app_idx);

/**
 * @brief         This function is called by Provisioner to get the local app key value.
 *
 * @param[in]     net_idx: Network key index.
 * @param[in]     app_idx: Application key index.
 *
 * @return        App key on success, or NULL on failure.
 *
 */
const uint8_t *esp_ble_mesh_provisioner_get_local_app_key(uint16_t net_idx, uint16_t app_idx);

/**
 * @brief         This function is called by Provisioner to bind own model with proper app key.
 *
 * @param[in]     element_addr: Provisioner local element address
 * @param[in]     app_idx: Provisioner local appkey index
 * @param[in]     model_id: Provisioner local model id
 * @param[in]     company_id: Provisioner local company id
 *
 * @note          company_id: If going to bind app_key with local vendor model, company_id
 *                            should be set to 0xFFFF.
 *
 * @return        ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_provisioner_bind_app_key_to_local_model(uint16_t element_addr, uint16_t app_idx,
        uint16_t model_id, uint16_t company_id);

/**
 * @brief         This function is called by Provisioner to add local network key.
 *
 * @param[in]     net_key: The network key to be added to the Provisioner local BLE Mesh stack.
 * @param[in]     net_idx: The network key index.
 *
 * @note          net_key: If set to NULL, net_key will be generated internally.
 *                net_idx: If it is going to be generated internally, it should be set to
 *                         0xFFFF, and the new net_idx will be reported via an event.
 *
 * @return        ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_provisioner_add_local_net_key(const uint8_t net_key[16], uint16_t net_idx);

/**
 * @brief         This function is called by Provisioner to update a local network key.
 *
 * @param[in]     net_key: Value of the NetKey.
 * @param[in]     net_idx: The NetKey Index.
 *
 * @return        ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_provisioner_update_local_net_key(const uint8_t net_key[16], uint16_t net_idx);

/**
 * @brief         This function is called by Provisioner to get the local network key value.
 *
 * @param[in]     net_idx: Network key index.
 *
 * @return        Network key on success, or NULL on failure.
 *
 */
const uint8_t *esp_ble_mesh_provisioner_get_local_net_key(uint16_t net_idx);

/**
 * @brief         This function is called by Provisioner to get provisioned node count.
 *
 * @return        Number of the provisioned nodes.
 *
 */
uint16_t esp_ble_mesh_provisioner_get_prov_node_count(void);

/**
 * @brief         This function is called by Provisioner to get the entry of the table of nodes.
 *
 * @note          After invoking the function to get the entry of nodes, users can use the "for"
 *                loop combined with the macro CONFIG_BLE_MESH_MAX_PROV_NODES to get each node's
 *                information. But before trying to read the node's information, users need to
 *                check if the node exists, i.e. if the *(esp_ble_mesh_node_t **node) is NULL.
 *
 * @return        Pointer to the start of nodes table.
 *
 */
esp_ble_mesh_node_t **esp_ble_mesh_provisioner_get_node_list_entry(void);

/**
 * @brief         This function is called by Provisioner to open the corresponding
 *                flash section for storing mesh provisioning information.
 *
 * @note          1. Provisioner can use this function to open a flash section for
 *                storing mesh information.
 *                2. And before calling any of restore, release and close functions,
 *                the open function must be called firstly.
 *
 * @param[in]     index: Provisioner settings index.
 *
 * @return        ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_provisioner_open_settings_with_index(uint8_t index);

/**
 * @brief         This function is called by Provisioner to open the corresponding
 *                flash section for storing mesh provisioning information.
 *
 * @note          1. Provisioner can use this function to open a flash section for
 *                storing mesh information.
 *                2. And before calling any of restore, release and close functions,
 *                the open function must be called firstly.
 *
 * @param[in]     user_id: User id of Provisioner settings.
 *
 * @return        ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_provisioner_open_settings_with_user_id(const char *user_id);

/**
 * @brief         This function is called by Provisioner to close the corresponding
 *                flash section which has been opened previously for storing mesh
 *                provisioning information.
 *
 * @note          1. Before closing the flash section, it must has been opened previously.
 *                2. When the release function is invoked, and the "erase" flag is set to
 *                false, then calling the close function will only close the flash section.
 *                And if the "erase" flag is set to true, besides closing the flash section,
 *                the corresponding settings user_id will also be cleaned and erased.
 *
 * @param[in]     index: Provisioner settings index.
 *
 * @return        ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_provisioner_close_settings_with_index(uint8_t index);

/**
 * @brief         This function is called by Provisioner to close the corresponding
 *                flash section which has been opened previously for storing mesh
 *                provisioning information.
 *
 * @note          1. Before closing the flash section, it must has been opened previously.
 *                2. When the release function is invoked, and the "erase" flag is set to
 *                false, then calling the close function will only close the flash section.
 *                And if the "erase" flag is set to true, besides closing the flash section,
 *                the corresponding settings user_id will also be cleaned and erased.
 *
 * @param[in]     user_id: User id of Provisioner settings.
 *
 * @return        ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_provisioner_close_settings_with_user_id(const char *user_id);

/**
 * @brief         This function is called by Provisioner to restore the mesh provisioning
 *                information from the corresponding flash section which has been opened
 *                previously.
 *
 * @note          1. Before calling this function to restore corresponding mesh information,
 *                the previously restored mesh information must be released using the release
 *                function.
 *                2. And the flash section must has been opened using the open function.
 *
 * @param[in]     index: Provisioner settings index.
 *
 * @return        ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_provisioner_restore_settings_with_index(uint8_t index);

/**
 * @brief         This function is called by Provisioner to restore the mesh provisioning
 *                information from the corresponding flash section which has been opened
 *                previously.
 *
 * @note          1. Before calling this function to restore corresponding mesh information,
 *                the previously restored mesh information must be released using the release
 *                function.
 *                2. And the flash section must has been opened using the open function.
 *
 * @param[in]     user_id: User id of Provisioner settings.
 *
 * @return        ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_provisioner_restore_settings_with_user_id(const char *user_id);

/**
 * @brief         This function is called by Provisioner to release the mesh provisioning
 *                information which has been restored from the corresponding flash section.
 *
 * @note          1. When this function is called, if the "erase" flag is set to false, the
 *                restored mesh information will be cleaned (e.g. removing the NetKey, etc).
 *                If the "erase" flag is set to true, besides cleaning the mesh information,
 *                the mesh information stored in the flash section will also be erased. And
 *                this function will also disable the Provisioner functionality internally.
 *                2. If Provisioner tries to work properly again, the restored function must
 *                be invoked to restore mesh information from this flash section if the mesh
 *                information is not erased, or from another flash section if erased.
 *                Before calling this, the open and restore functions must be invoked.
 *                3. The whole working process of Provisioner settings should be as following:
 *                a) open settings A
 *                b) restore settings A
 *                c) start to provision and control nodes
 *                d) release settings A
 *                e) close settings A
 *                f) open settings B
 *                g) restore settings B
 *                h) start to provision and control other nodes
 *                i) release settings B
 *                j) close settings B
 *                k) ......
 *
 * @param[in]     index: Provisioner settings index.
 * @param[in]     erase: Indicate whether erase the information from flash.
 *
 * @return        ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_provisioner_release_settings_with_index(uint8_t index, bool erase);

/**
 * @brief         This function is called by Provisioner to release the mesh provisioning
 *                information which has been restored from the corresponding flash section.
 *
 * @note          1. When this function is called, if the "erase" flag is set to false, the
 *                restored mesh information will be cleaned (e.g. removing the NetKey, etc).
 *                If the "erase" flag is set to true, besides cleaning the mesh information,
 *                the mesh information stored in the flash section will also be erased. And
 *                this function will also disable the Provisioner functionality internally.
 *                2. If Provisioner tries to work properly again, the restored function must
 *                be invoked to restore mesh information from this flash section if the mesh
 *                information is not erased, or from another flash section if erased.
 *                Before calling this, the open and restore functions must be invoked.
 *                3. The whole working process of Provisioner settings should be as following:
 *                a) open settings A
 *                b) restore settings A
 *                c) start to provision and control nodes
 *                d) release settings A
 *                e) close settings A
 *                f) open settings B
 *                g) restore settings B
 *                h) start to provision and control other nodes
 *                i) release settings B
 *                j) close settings B
 *                k) ......
 *
 * @param[in]     user_id: User id of Provisioner settings.
 * @param[in]     erase: Indicate whether erase the information from flash.
 *
 * @return        ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_provisioner_release_settings_with_user_id(const char *user_id, bool erase);

/**
 * @brief         This function is called by Provisioner to erase the mesh provisioning
 *                information which is not been restored, and the corrseponding settings
 *                user_id from the flash section.
 *
 * @note          When this function is called, the corresponding nvs namespace must not
 *                be open and restored. This function is used to erase the mesh information
 *                and settings user_id which are not been used currently.
 *
 * @param[in]     index: Provisioner settings index.
 *
 * @return        ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_provisioner_delete_settings_with_index(uint8_t index);

/**
 * @brief         This function is called by Provisioner to erase the mesh provisioning
 *                information which is not been restored, and the corrseponding settings
 *                user_id from the flash section.
 *
 * @note          When this function is called, the corresponding nvs namespace must not
 *                be open and restored. This function is used to erase the mesh information
 *                and settings user_id which are not been used currently.
 *
 * @param[in]     index: Provisioner settings index.
 *
 * @return        ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_provisioner_delete_settings_with_user_id(const char *user_id);

/**
 * @brief         This function is called by Provisioner to get the settings user id.
 *
 * @param[in]     index: Provisioner settings index.
 *
 * @return        User id on success or NULL on failure.
 *
 */
const char *esp_ble_mesh_provisioner_get_settings_user_id(uint8_t index);

/**
 * @brief         This function is called by Provisioner to get the settings index.
 *
 * @param[in]     user_id: User id of Provisioner settings.
 *
 * @return        Provisioner Settings index.
 *
 */
uint8_t esp_ble_mesh_provisioner_get_settings_index(const char *user_id);

/**
 * @brief         This function is called by Provisioner to get the number of free
 *                settings user_id.
 *
 * @return        Number of free Provisioner Settings.
 *
 */
uint8_t esp_ble_mesh_provisioner_get_free_settings_user_id_count(void);

/**
 * @brief         This function is called by Provisioner to directly erase the stored
 *                mesh information from flash.
 *
 * @note          This function can be invoked when the mesh stack is not initialized
 *                or has been deinitialized.
 *
 * @return        ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_provisioner_direct_erase_settings(void);

/**
 * @brief         This function is called by Provisioner to start receiving and processing
 *                heartbeat messages.
 *
 * @note          If starting receiving heartbeat message successfully, the filter will be
 *                an empty blacklist, which means all the heartbeat messages received by
 *                Provisioner will be processed and reported to the application layer.
 *
 * @return        ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_provisioner_start_recv_heartbeat(void);

/**
 * @brief         This function is called by Provisioner to set the heartbeat filter type.
 *
 * @note          1. If the filter type is not the same with the current value, then all the
 *                addresses in the filter will be cleared.
 *                2. If the previous type is blacklist, and changed to whitelist, then the
 *                filter will be an empty whitelist, which means no heartbeat messages will
 *                be reported. And users need to add source addresses into the filter, then
 *                heartbeat messages from these addresses will be reported.
 *
 * @param[in]     filter_type: Heartbeat filter type (whitelist or blacklist).
 *
 * @return        ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_provisioner_set_heartbeat_filter_type(uint8_t filter_type);

/**
 * @brief         This function is called by Provisioner to add, remove or clean the corresponding
 *                information from the heartbeat filter.
 *
 * @note          1. If the op_flag is "add", the hb_src can be set to the source address (can only
 *                be a unicast address) of heartbeat messages, and the hb_dst can be set to the
 *                destination address (unicast address or group address), at least one of them needs
 *                to be set.
 *                If only one of them is set, the filter entry will only use the configured source
 *                address or destination address to filter heartbeat messages. If both of them are
 *                set, then the source address and destination address will both be used to decide
 *                if a heartbeat message can be reported.
 *                And when the filter is whitelist, users can use the expiry (in seconds) to decide
 *                how long the filter entry can be used to filter heartbeat messages. If the expiry
 *                is set to 0, then the corresponding filter entry will be valid indefinitely. Only
 *                when the filter entry is removed, cleaned or the filter_type is changed (whitelist
 *                to blacklist), then the filter entry will be invalid.
 *                If part of the filter information already exists, then the corresponding filter
 *                entry will be updated. For example, if the source address already exists, and users
 *                try to add a filter entry with the same source address. In this situation, the
 *                existed filter entry will be updated. The same for the destination address.
 *                And if the source address and destination address are both set, then all the filter
 *                entries which contain any of the two addresses will be cleaned. After this, a new
 *                filter entry will be allocated to store the filter information.
 *                2. If the op_flag is "remove", the hb_src can be set to the source address (can only
 *                be a unicast address) of heartbeat messages, and the hb_dst can be set to the
 *                destination address (unicast address or group address), at least one of them needs
 *                to be set. If only one of the two addresses is set, then the filter entry with the
 *                same source address or destination address will be removed. And if both of them are
 *                set, then only the filter entry with the same source address and destination address
 *                will be removed. User don't need to set the expiry parameter.
 *                3. If the op_flag is "clean", then Provisioner will remove all the information from
 *                each heartbeat filter entry, users don't need to set the parameter info.
 *
 * @param[in]     op_flag: Add, remove or clean
 * @param[in]     info:    Pointer to heartbeat filter entry information, the information includes:
                           hb_src - Heartbeat source address;
                           hb_dst - Heartbeat destination address;
                           expiry - Period (in seconds) for receiving heartbeat messages
 *
 * @return        ESP_OK on success or error code otherwise.
 *
 */
esp_err_t esp_ble_mesh_provisioner_set_heartbeat_filter_info(uint8_t op_flag, esp_ble_mesh_provisioner_hb_filter_info_t *info);

/**
 * @brief         This function is called to get fast provisioning application key.
 *
 * @param[in]     net_idx: Network key index.
 * @param[in]     app_idx: Application key index.
 *
 * @return        Application key on success, or NULL on failure.
 *
 */
const uint8_t *esp_ble_mesh_get_fast_prov_app_key(uint16_t net_idx, uint16_t app_idx);

#endif /* _ESP_BLE_MESH_NETWORKING_API_H_ */
