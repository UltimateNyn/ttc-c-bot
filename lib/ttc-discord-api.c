#include "ttc-discord/discord.h"
#include <json-c/json_tokener.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ttc-discord/api.h>
#include <discord.h>
#include <json-c/json_object.h>

#include <ttc-log.h>
#include <ttc-ws.h>
#include <errno.h>
#include <ttc-http.h>

#include <json-c/json.h>

int ttc_discord_message_extract_embed(ttc_discord_ctx_t *ctx, snowflake_t cid, snowflake_t mid,
		ttc_discord_embed_t *embed) {
	ttc_http_request_t *request;
	ttc_http_response_t *response;
	json_object *message, *author, *embeds, *field, *em;
	char *url = NULL;
	int length; 


	length = snprintf(NULL, 0, "/api/v10/channels/%lu/messages/%lu", cid, mid);
	
	url = calloc(1, length + 1);

	length = snprintf(url, length + 1, "/api/v10/channels/%lu/messages/%lu", cid, mid);
	
	request = ttc_http_new_request();
	ttc_http_request_set_path(request, url);
	ttc_http_request_set_http_version(request, HTTP_VER_11);
	ttc_http_request_set_method(request, TTC_HTTP_METHOD_GET);

	response = ttc_discord_api_send_request(ctx, request);
	TTC_LOG_WARN("%s\n", response->data);
	printf("%s\n", ttc_http_request_get_str(request));
	


	length = response->status;
	if(length == 200) {

		message = json_tokener_parse(response->data);

		json_object_object_get_ex(message, "author", &author);
		if(strcmp(json_object_get_string(json_object_object_get(author, "id")), ctx->app_id) != 0) {
			TTC_LOG_ERROR("The is not a message by the bot can't decode this\n");
			return -1;
		}

		json_object_object_get_ex(message, "embeds", &embeds);
		em = json_object_array_get_idx(embeds, 0);

		embed->title = strdup(json_object_get_string(json_object_object_get(em, "title")));
		embed->description = strdup(json_object_get_string(json_object_object_get(em, "description")));
		return  0;
	}


	return 1;
}

int ttc_discord_create_button(ttc_discord_ctx_t *ctx) {
	ttc_http_request_t *request;
	ttc_http_response_t *response;
	char *length_str;
	json_object *message, *components, *button, *type,
				*label, *style, *id, *row, *artype, *arcomponents;
	
	artype = json_object_new_int(DiscordComponentActionRow);
	row = json_object_new_object();
	arcomponents = json_object_new_array();

	json_object_object_add(row, "type", artype);
	json_object_object_add(row, "components", arcomponents);

	components = json_object_new_array();
	message = json_object_new_object();
	button = json_object_new_object();
	type = json_object_new_int(2);
	label = json_object_new_string("Create ticket!");
	id = json_object_new_string("create_ticket");
	style = json_object_new_int(1);

	json_object_object_add(button, "type", type);
	json_object_object_add(button, "custom_id", id);
	json_object_object_add(button, "label", label);
	json_object_object_add(button, "style", style);
	json_object_array_add(components, row);
	json_object_array_add(arcomponents, button);
	json_object_object_add(message, "components", components);


	int length = snprintf(NULL, 0, "%lu", strlen(json_object_to_json_string(message)));

	length_str = calloc(1, length + 1);
	snprintf(length_str, length + 1, "%lu", strlen(json_object_to_json_string(message)));
	TTC_LOG_DEBUG("%s\n", json_object_to_json_string(message));



	request = ttc_http_new_request();
	ttc_http_request_set_path(request, "/api/v10/channels/913091622592458833/messages");
	ttc_http_request_set_http_version(request, HTTP_VER_11);
	ttc_http_request_set_method(request, TTC_HTTP_METHOD_POST);
	
	response = ttc_discord_api_send_json(ctx, request, message);
	TTC_LOG_DEBUG("Response Code: %d\n%s\n",response->status, response->data);

	return 0;
}

int ttc_discord_send_modal() {
	json_object *id, *title, *components, *row, *submit,
				*title_input, *sysinfo, *description,
				*row_type, *title_type, *title_style,
				*sysinfo_type, *sysinfo_style,
				*desc_type, *desc_style, *title_id,
				*label, *modal_id,
				*catergory, *modal, *row_components;

	modal = json_object_new_object();
	modal_id = json_object_new_string("ticket_modal");
	json_object_object_add(modal, "custom_id", modal_id);
	components = json_object_new_array();
	json_object_object_add(modal, "components", components);

	row_components = json_object_new_array();
	id = json_object_new_string("ticket_submit");
	row = json_object_new_object();
	row_type = json_object_new_int(DiscordComponentActionRow);

	json_object_object_add(row, "type", row_type);
	json_object_object_add(row, "components", row_components);
	json_object_array_add(components, row);

	title_input = json_object_new_object();
	title = json_object_new_string("Create Ticket:");
	json_object_object_add(modal, "title", title);
	label = json_object_new_string("Ticket Title:");
	title_type = json_object_new_int(DiscordComponentTextInput);
	title_style = json_object_new_int(DiscordTextInputSingleLine);
	
	json_object_object_add(title_input, "type", title_type);
	json_object_object_add(title_input, "style", title_style);
	json_object_object_add(title_input, "label", label);
	json_object_object_add(title_input, "custom_id", title_id);



	return 0;
}
