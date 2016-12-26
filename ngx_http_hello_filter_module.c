#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>


typedef struct {
    ngx_flag_t enable;
} ngx_http_hello_conf_t;


static ngx_int_t ngx_http_hello_filter_init(ngx_conf_t *cf);
static void *ngx_http_hello_create_conf(ngx_conf_t *cf);
static char *ngx_http_hello_merge_conf(ngx_conf_t *cf,
    void *parent, void *child);


static ngx_command_t ngx_http_hello_filter_commands[] = {

    { ngx_string("hello"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_FLAG,
      ngx_conf_set_flag_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_hello_conf_t, enable),
      NULL},

      ngx_null_command
};


static ngx_http_module_t ngx_http_hello_filter_module_ctx = {
    NULL,                                  /* preconfiguration */
    ngx_http_hello_filter_init,            /* postconfiguration */

    NULL,                                  /* create main configuration */
    NULL,                                  /* init main configuration */

    NULL,                                  /* create server configuration */
    NULL,                                  /* merge server configuration */

    ngx_http_hello_create_conf,            /* create location configuration */
    ngx_http_hello_merge_conf              /* merge location configuration */
};


ngx_module_t ngx_http_hello_filter_module = {
    NGX_MODULE_V1,
    &ngx_http_hello_filter_module_ctx,     /* module context */
    ngx_http_hello_filter_commands,        /* module directives */
    NGX_HTTP_MODULE,                       /* module type */
    NULL,                                  /* init master */
    NULL,                                  /* init module */
    NULL,                                  /* init process */
    NULL,                                  /* init thread */
    NULL,                                  /* exit thread */
    NULL,                                  /* exit process */
    NULL,                                  /* exit master */
    NGX_MODULE_V1_PADDING
};


static ngx_http_output_header_filter_pt ngx_http_next_header_filter;


static ngx_int_t
ngx_http_hello_filter(ngx_http_request_t *r)
{
    ngx_table_elt_t *h;
    ngx_http_hello_conf_t *conf;

    conf = ngx_http_get_module_loc_conf(r, ngx_http_hello_filter_module);

    if (conf->enable) {
        h = ngx_list_push(&r->headers_out.headers);
        if (h == NULL) {
            return NGX_ERROR;
        }

        h->hash = 1;
        ngx_str_set(&h->key, "Hello");
        ngx_str_set(&h->value, "world");
    }

    return ngx_http_next_header_filter(r);
}


static ngx_int_t
ngx_http_hello_filter_init(ngx_conf_t *cf)
{
    ngx_http_next_header_filter = ngx_http_top_header_filter;
    ngx_http_top_header_filter = ngx_http_hello_filter;

    return NGX_OK;
}


static void *
ngx_http_hello_create_conf(ngx_conf_t *cf)
{
    ngx_http_hello_conf_t *conf;

    conf = ngx_pcalloc(cf->pool, sizeof(ngx_http_hello_conf_t));
    if (conf == NULL) {
        return NULL;
    }

    conf->enable = NGX_CONF_UNSET;

    return conf;
}


static char *
ngx_http_hello_merge_conf(ngx_conf_t *cf, void *parent, void *child)
{
    ngx_http_hello_conf_t *prev = parent;
    ngx_http_hello_conf_t *conf = child;

    ngx_conf_merge_value(conf->enable, prev->enable, 0);

    return NGX_CONF_OK;
}
