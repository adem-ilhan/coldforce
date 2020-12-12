#include <coldforce/core/co_std.h>
#include <coldforce/core/co_string.h>

#include <coldforce/http/co_http_response.h>

//---------------------------------------------------------------------------//
// http response
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//---------------------------------------------------------------------------//

void
co_http_response_serialize(
    const co_http_response_t* response,
    co_byte_array_t* buffer
)
{
    co_byte_array_add_string(buffer, response->version);
    co_byte_array_add_string(buffer, CO_HTTP_SP);

    char status_code[32];
    sprintf(status_code, "%hu", response->status_code);

    co_byte_array_add_string(buffer, status_code);
    co_byte_array_add_string(buffer, CO_HTTP_SP);
    co_byte_array_add_string(buffer, response->reason_phrase);
    co_byte_array_add_string(buffer, CO_HTTP_CRLF);

    co_http_message_serialize(&response->message, buffer);
}

int
co_http_response_deserialize(
    co_http_response_t* response,
    const co_byte_array_t* data,
    size_t* index
)
{
    const char* data_ptr =
        (const char*)co_byte_array_get_ptr((co_byte_array_t*)data, *index);
    const size_t data_size = co_byte_array_get_count(data) - (*index);

    const char* new_line =
        co_string_find_n(data_ptr, CO_HTTP_CRLF, data_size);

    if (new_line == NULL)
    {
        return CO_HTTP_PARSE_MORE_DATA;
    }

    size_t length = (new_line - data_ptr);
    size_t item_length = 0;
    size_t temp_index = 0;

    length -= temp_index;

    const char* sp =
        co_string_find_n(&data_ptr[temp_index], " ", length);

    if (sp == NULL)
    {
        return CO_HTTP_PARSE_ERROR;
    }

    item_length = sp - &data_ptr[temp_index];

    char* version =
        co_string_duplicate_n(&data_ptr[temp_index], item_length);

    length -= (item_length + 1);
    temp_index += (item_length + 1);

    sp = co_string_find_n(&data_ptr[temp_index], " ", length);

    if (sp == NULL)
    {
        co_mem_free(version);

        return CO_HTTP_PARSE_ERROR;
    }

    item_length = sp - &data_ptr[temp_index];

    char* status_code_str =
        co_string_duplicate_n(&data_ptr[temp_index], item_length);

    size_t status_code =
        co_string_to_size_t(status_code_str, NULL, 10);

    co_mem_free(status_code_str);

    length -= (item_length + 1);
    temp_index += (item_length + 1);

    item_length = length;

    char* reason_phrase =
        co_string_duplicate_n(&data_ptr[temp_index], item_length);

    response->version = version;
    response->status_code = (uint16_t)status_code;
    response->reason_phrase = reason_phrase;

    (*index) += (new_line - data_ptr) + CO_HTTP_CRLF_LENGTH;

    return co_http_message_deserialize_header(data, index, &response->message);
}

void
co_http_response_print_header(
    const co_http_response_t* response
)
{
    if (response != NULL)
    {
        printf("--------\n");

        printf("%s %d %s\n",
            co_http_response_get_version(response),
            co_http_response_get_status_code(response),
            co_http_response_get_reason_phrase(response));

        co_http_header_print(&response->message.header);

        printf("--------\n");
    }
}

//---------------------------------------------------------------------------//
//---------------------------------------------------------------------------//

co_http_response_t*
co_http_response_create(
    void
)
{
    co_http_response_t* response =
        (co_http_response_t*)co_mem_alloc(sizeof(co_http_response_t));

    if (response == NULL)
    {
        return NULL;
    }

    co_http_message_setup(&response->message);

    response->version = NULL;
    response->status_code = 0;
    response->reason_phrase = NULL;

    return response;
}

co_http_response_t*
co_http_response_create_with(
    uint16_t status_code,
    const char* reason_phrase
)
{
    co_http_response_t* response = co_http_response_create();

    if (response == NULL)
    {
        return NULL;
    }

    co_http_response_set_status_code(response, status_code);
    co_http_response_set_reason_phrase(response, reason_phrase);

    return response;
}

void
co_http_response_destroy(
    co_http_response_t* response
)
{
    if (response != NULL)
    {
        co_mem_free(response->version);
        co_mem_free(response->reason_phrase);

        co_http_message_cleanup(&response->message);

        co_mem_free(response);
    }
}

co_http_header_t*
co_http_response_get_header(
    co_http_response_t* response
)
{
    return &response->message.header;
}

const co_http_header_t*
co_http_response_get_const_header(
    const co_http_response_t* response
)
{
    return &response->message.header;
}

bool
co_http_response_set_content(
    co_http_response_t* response,
    const void* data,
    size_t data_size
)
{
    return co_http_message_set_content(
        &response->message, data, data_size);
}

const void*
co_http_response_get_content(
    const co_http_response_t* response
)
{
    return co_http_message_get_content(
        &response->message);
}

size_t
co_http_response_get_content_size(
    const co_http_response_t* response
)
{
    return co_http_message_get_content_size(
        &response->message);
}

void
co_http_response_set_version(
    co_http_response_t* response,
    const char* version
)
{
    co_mem_free(response->version);

    if (version != NULL)
    {
        response->version = co_string_duplicate(version);
    }
    else
    {
        response->version = NULL;
    }
}

const char*
co_http_response_get_version(
    const co_http_response_t* response
)
{
    return response->version;
}

void
co_http_response_set_status_code(
    co_http_response_t* response,
    uint16_t status_code
)
{
    response->status_code = status_code;
}

uint16_t
co_http_response_get_status_code(
    const co_http_response_t* response
)
{
    return response->status_code;
}

void
co_http_response_set_reason_phrase(
    co_http_response_t* response,
    const char* reason_phrase
)
{
    co_mem_free(response->reason_phrase);

    if (reason_phrase != NULL)
    {
        response->reason_phrase = co_string_duplicate(reason_phrase);
    }
    else
    {
        response->reason_phrase = NULL;
    }
}

const char*
co_http_response_get_reason_phrase(
    const co_http_response_t* response
)
{
    return response->reason_phrase;
}

void
co_http_response_add_cookie(
    co_http_response_t* response,
    co_http_cookie_st* cookie
)
{
    co_byte_array_t* buffer = co_byte_array_create();

    co_http_response_cookie_serialize(cookie, buffer);

    co_byte_array_add(buffer, "\0", 1);
    char* str = (char*)co_byte_array_detach(buffer);

    co_http_header_add_item(
        &response->message.header, CO_HTTP_HEADER_SET_COOKIE, str);

    co_mem_free(str);
    co_byte_array_destroy(buffer);
}

size_t
co_http_response_get_cookies(
    const co_http_response_t* response,
    co_http_cookie_st* cookies,
    size_t count
)
{
    const co_http_header_t* header = &response->message.header;

    co_list_iterator_t* it =
        co_list_get_head_iterator(header->items);

    size_t index = 0;

    while ((it != NULL) && (index < count))
    {
        const co_list_data_st* data =
            co_list_get_next(header->items, &it);

        const co_http_header_item_t* item =
            (const co_http_header_item_t*)data->value;

        if (co_string_case_compare(
            item->name, CO_HTTP_HEADER_SET_COOKIE) == 0)
        {
            if (co_http_response_cookie_deserialize(
                item->value, &cookies[index]))
            {
                ++index;
            }
        }
    }

    return index;
}

void
co_http_response_remove_all_cookies(
    co_http_response_t* response
)
{
    co_http_header_remove_all_values(
        &response->message.header, CO_HTTP_HEADER_SET_COOKIE);
}
