#include "stdio.h"

#include "z_soap/soapH.h"
#include "z_soap/soapzcsPortBindingProxy.h"
#include "z_soap/zcsPortBinding.nsmap"

int main ()
{
    std::string pw ("[e`dsqgfhjkm");
    ns4__authRequest authRequest;
    ns4__authResponse authResponse;
    ns3__accountSelector accountSelector;
    accountSelector.__item = "sheridan";
    accountSelector.by = ns3__accountBy__name;
    printf("Variables prepared\n");
    zcsPortBinding *zpb = new zcsPortBinding();
    zpb->endpoint = "http://mail.ordzhonikidze-sanatorium.ru/service/soap/";
    printf("Proxy created\n");
    authRequest.account  = &accountSelector;
    authRequest.password = &pw;
    //authRequest.prefs
    printf("Request prepared\n");
    int result = zpb->__ns1__authRequest(&authRequest, &authResponse);
    printf("Request executed, %s:%d\n", result == SOAP_OK? "ok" : "no ok", result);
    printf("Result. Token: %s, lifetime: %ld\n", authResponse.authToken.c_str(), authResponse.lifetime);
    delete zpb;
    return 0;
}
