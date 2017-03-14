#include <stdio.h>
#include <stdlib.h>
#include <ldap.h>
#include <sasl/sasl.h>

typedef struct
{
  char *username;
  char *password;
  char *bind_dn;
} my_authdata;

int my_sasl_interact(LDAP *ld, unsigned flags, void *defaults, void *in)
{
  my_authdata *auth = (my_authdata *)defaults;
  sasl_interact_t *interact = (sasl_interact_t *)in;
  if(ld == NULL) return LDAP_PARAM_ERROR;

  while(interact->id != SASL_CB_LIST_END)
  {
    char *dflt = (char *)interact->defresult;
    fprintf(stdout, "Interact id: %d\n", interact->id);
    switch(interact->id)
    {
      case SASL_CB_GETREALM:
        dflt = NULL;
        fprintf(stdout, "Got SASL_CB_GETREALM\n");
        break;
      case SASL_CB_USER:
        fprintf(stdout, "Got SASL_CB_USER\n");
        //dflt = auth->username;
        //break;
      case SASL_CB_AUTHNAME:
        fprintf(stdout, "Got SASL_CB_AUTHNAME\n");
        dflt = auth->bind_dn;
        break;
      case SASL_CB_PASS:
        dflt = auth->password;
        fprintf(stdout, "Got SASL_CB_PASS\n");
        break;
      default:
        printf("my_sasl_interact asked for unknown %ld\n",interact->id);
    }
    fprintf(stdout, "send %s\n", dflt);
    interact->result = (dflt && *dflt) ? dflt : (char *)"";
    interact->len = strlen((char *)interact->result);
    interact++;
  }

  return LDAP_SUCCESS;
}

int main(int argc, char *argv[])
{
  int rc;
  LDAP *ld = NULL;

  static my_authdata auth;
  auth.username = "maxim.gorlov";
  //auth.username = "maxim.gorlov@sergos.ru";
  //auth.username = "uid=maxim.gorlov,ou=people,dc=ordzhonikidze-sanatorium,dc=ru";
  //auth.bind_dn = "dc=ordzhonikidze-sanatorium,dc=ru";
  auth.bind_dn = "uid=zmpostfix,cn=appaccts,cn=maxim.gorlov";
  auth.password = "[e`dsqgfhjkm";

  char *sasl_mech = ber_strdup("DIGEST-MD5");
  char *ldapuri = ber_strdup("ldap://mail.sergos.ru");

  int protocol = LDAP_VERSION3;
  unsigned sasl_flags = LDAP_SASL_INTERACTIVE;
  //char *binddn = "ou=people,dc=ordzhonikidze-sanatorium,dc=ru\0";
  char *binddn  = NULL;

  rc = ldap_initialize(&ld, ldapuri);
  if(rc != LDAP_SUCCESS)
  {
    fprintf(stderr, "ldap_initialize: %s\n", ldap_err2string(rc));
    return rc;
  }
  fprintf(stdout, "ldap_initialize done\n");


  if(ldap_set_option(ld, LDAP_OPT_PROTOCOL_VERSION, &protocol) != LDAP_OPT_SUCCESS)
  {
    fprintf(stderr, "Could not set LDAP_OPT_PROTOCOL_VERSION %d\n", protocol);
    return -1;
  }
  fprintf(stdout, "ldap_set_option done\n");

  rc = ldap_sasl_interactive_bind_s(ld,
                                    binddn,
                                    sasl_mech,
                                    NULL,
                                    NULL,
                                    sasl_flags,
                                    my_sasl_interact,
                                    &auth);
  //rc = ldap_simple_bind_s(ld, auth.username, auth.password);
  //rc = ldap_gssapi_bind_s(ld, auth.username, auth.password);

  if(rc != LDAP_SUCCESS)
  {
    //ldap_perror(ld, "ldap_sasl_interactive_bind_s");
    fprintf(stderr, "ldap *_bind_s: %s\n", ldap_err2string(rc));
    ldap_unbind_ext_s(ld, NULL, NULL);
    return rc;
  }

  fprintf(stdout, "Authentication succeeded\n");

  rc = ldap_unbind_ext_s(ld, NULL, NULL);
  sasl_done();
  sasl_client_init(NULL);

  return rc;
}
