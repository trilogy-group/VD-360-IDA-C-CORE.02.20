
















    //////////////////////////////////////////
    //                                      //
    //  THIS FILE IS AUTOMATICLY GENERATED  //
    //                                      //
    //  ****  DO NOT MODIFY DIRECTLY  ****  //
    //                                      //
    //         (modify gencode.pl)          //
    //                                      //
    //////////////////////////////////////////
























 
//CB>---------------------------------------------------------------------------
//
//   File, Component, Release:
//                  TdfProcess/IdaStringToEnum.cc 1.0 12-APR-2008 18:52:14 DMSYS
//
//   File:      TdfProcess/IdaStringToEnum.cc
//   Revision:      1.0
//   Date:          12-APR-2008 18:52:14
//
//   DESCRIPTION:
//     
//     
//
//
//<CE---------------------------------------------------------------------------

static const char * SCCS_Id_IdaStringToEnum_cc = "@(#) TdfProcess/IdaStringToEnum.cc 1.0 12-APR-2008 18:52:14 DMSYS";


#include <string.h>
#include <pcpdefs.h>
#include <IdaStringToEnum.h>
#include <idatraceman.h>


ReturnStatus StringToEnum::enumOfOperation(ULong& enumValue, const char* strValue)
{
	if (strcmp(strValue, "initialSearch") == 0) { enumValue = 0; return isOk; }
	if (strcmp(strValue, "nextPage") == 0) { enumValue = 1; return isOk; }
	if (strcmp(strValue, "zoomInSearch") == 0) { enumValue = 2; return isOk; }
	if (strcmp(strValue, "neighbourhoodSearch") == 0) { enumValue = 3; return isOk; }
	if (strcmp(strValue, "directSearch") == 0) { enumValue = 4; return isOk; }
	if (strcmp(strValue, "previousPage") == 0) { enumValue = 5; return isOk; }
	if (strcmp(strValue, "zoomOutSearch") == 0) { enumValue = 6; return isOk; }
	if (strcmp(strValue, "navigateForward") == 0) { enumValue = 7; return isOk; }
	if (strcmp(strValue, "navigateBackward") == 0) { enumValue = 8; return isOk; }
	if (strcmp(strValue, "fullSetSearch") == 0) { enumValue = 9; return isOk; }
	if (strcmp(strValue, "nextHit") == 0) { enumValue = 10; return isOk; }
	if (strcmp(strValue, "basicSearch") == 0) { enumValue = 11; return isOk; }
	if (strcmp(strValue, "backHistory") == 0) { enumValue = 12; return isOk; }
	if (strcmp(strValue, "displayRecord") == 0) { enumValue = 13; return isOk; }
	idaTrackExcept(("StringToEnum::enumOfOperation(): invalid parameter \"%s\"",strValue));
	return isNotOk;
}

ReturnStatus StringToEnum::enumOfModifyOperation(ULong& enumValue, const char* strValue)
{
	if (strcmp(strValue, "modifyRecord") == 0) { enumValue = 0; return isOk; }
	if (strcmp(strValue, "deleteRecord") == 0) { enumValue = 1; return isOk; }
	if (strcmp(strValue, "restoreRecord") == 0) { enumValue = 2; return isOk; }
	if (strcmp(strValue, "insertRecord") == 0) { enumValue = 3; return isOk; }
	idaTrackExcept(("StringToEnum::enumOfModifyOperation(): invalid parameter \"%s\"",strValue));
	return isNotOk;
}

ReturnStatus StringToEnum::enumOfAttributeId(ULong& enumValue, const char* strValue)
{
	if (strcmp(strValue, "name") == 0) { enumValue = 1; return isOk; }
	if (strcmp(strValue, "givenName") == 0) { enumValue = 2; return isOk; }
	if (strcmp(strValue, "streetName") == 0) { enumValue = 3; return isOk; }
	if (strcmp(strValue, "houseNumber") == 0) { enumValue = 4; return isOk; }
	if (strcmp(strValue, "maidenName") == 0) { enumValue = 5; return isOk; }
	if (strcmp(strValue, "subordinateSearchInformation") == 0) { enumValue = 6; return isOk; }
	if (strcmp(strValue, "businessCategoryOrProfession") == 0) { enumValue = 7; return isOk; }
	if (strcmp(strValue, "addressText") == 0) { enumValue = 8; return isOk; }
	if (strcmp(strValue, "zipCode") == 0) { enumValue = 9; return isOk; }
	if (strcmp(strValue, "localityName") == 0) { enumValue = 10; return isOk; }
	if (strcmp(strValue, "telephoneNumberDirectiveText") == 0) { enumValue = 11; return isOk; }
	if (strcmp(strValue, "areaCode") == 0) { enumValue = 12; return isOk; }
	if (strcmp(strValue, "telephoneNumber") == 0) { enumValue = 13; return isOk; }
	if (strcmp(strValue, "faxNumber") == 0) { enumValue = 15; return isOk; }
	if (strcmp(strValue, "facsimileNumber") == 0) { enumValue = 15; return isOk; }
	if (strcmp(strValue, "eMailAddress") == 0) { enumValue = 16; return isOk; }
	if (strcmp(strValue, "profession") == 0) { enumValue = 17; return isOk; }
	if (strcmp(strValue, "quarterName") == 0) { enumValue = 18; return isOk; }
	if (strcmp(strValue, "extension") == 0) { enumValue = 20; return isOk; }
	if (strcmp(strValue, "billingInformation") == 0) { enumValue = 21; return isOk; }
	if (strcmp(strValue, "stateName") == 0) { enumValue = 22; return isOk; }
	if (strcmp(strValue, "regionName") == 0) { enumValue = 23; return isOk; }
	if (strcmp(strValue, "countryName") == 0) { enumValue = 24; return isOk; }
	if (strcmp(strValue, "ypSupAddress") == 0) { enumValue = 29; return isOk; }
	if (strcmp(strValue, "ypSubAddress") == 0) { enumValue = 29; return isOk; }
	if (strcmp(strValue, "ypSubAdress") == 0) { enumValue = 29; return isOk; }
	if (strcmp(strValue, "dataProvider") == 0) { enumValue = 30; return isOk; }
	if (strcmp(strValue, "bookName") == 0) { enumValue = 38; return isOk; }
	if (strcmp(strValue, "cityName") == 0) { enumValue = 39; return isOk; }
	if (strcmp(strValue, "url") == 0) { enumValue = 40; return isOk; }
	if (strcmp(strValue, "nameText") == 0) { enumValue = 41; return isOk; }
	if (strcmp(strValue, "dsaSubAddress") == 0) { enumValue = 42; return isOk; }
	if (strcmp(strValue, "dsaSubAdress") == 0) { enumValue = 42; return isOk; }
	if (strcmp(strValue, "coordinateX") == 0) { enumValue = 43; return isOk; }
	if (strcmp(strValue, "subscriberCoordsX") == 0) { enumValue = 43; return isOk; }
	if (strcmp(strValue, "coordinateY") == 0) { enumValue = 44; return isOk; }
	if (strcmp(strValue, "subscriberCoordsY") == 0) { enumValue = 44; return isOk; }
	if (strcmp(strValue, "countryCode") == 0) { enumValue = 54; return isOk; }
	if (strcmp(strValue, "telephoneNumberPlusAreaCode") == 0) { enumValue = 55; return isOk; }
	if (strcmp(strValue, "directoryType") == 0) { enumValue = 56; return isOk; }
	if (strcmp(strValue, "andOpen") == 0) { enumValue = 59; return isOk; }
	if (strcmp(strValue, "andClose") == 0) { enumValue = 60; return isOk; }
	if (strcmp(strValue, "orOpen") == 0) { enumValue = 61; return isOk; }
	if (strcmp(strValue, "orClose") == 0) { enumValue = 62; return isOk; }
	if (strcmp(strValue, "negation") == 0) { enumValue = 63; return isOk; }
	if (strcmp(strValue, "transSource") == 0) { enumValue = 64; return isOk; }
	if (strcmp(strValue, "transDest") == 0) { enumValue = 65; return isOk; }
	if (strcmp(strValue, "column") == 0) { enumValue = 66; return isOk; }
	if (strcmp(strValue, "columnId") == 0) { enumValue = 67; return isOk; }
	if (strcmp(strValue, "topic") == 0) { enumValue = 68; return isOk; }
	if (strcmp(strValue, "shortcut") == 0) { enumValue = 69; return isOk; }
	if (strcmp(strValue, "text") == 0) { enumValue = 70; return isOk; }
	if (strcmp(strValue, "searchAttr1") == 0) { enumValue = 110; return isOk; }
	if (strcmp(strValue, "searchAttr2") == 0) { enumValue = 111; return isOk; }
	if (strcmp(strValue, "searchAttr3") == 0) { enumValue = 112; return isOk; }
	if (strcmp(strValue, "searchAttr4") == 0) { enumValue = 113; return isOk; }
	if (strcmp(strValue, "searchAttr5") == 0) { enumValue = 114; return isOk; }
	if (strcmp(strValue, "searchAttr6") == 0) { enumValue = 115; return isOk; }
	if (strcmp(strValue, "searchAttr7") == 0) { enumValue = 116; return isOk; }
	if (strcmp(strValue, "searchAttr8") == 0) { enumValue = 117; return isOk; }
	if (strcmp(strValue, "searchAttr9") == 0) { enumValue = 118; return isOk; }
	if (strcmp(strValue, "searchAttr10") == 0) { enumValue = 119; return isOk; }
	if (strcmp(strValue, "searchAttr11") == 0) { enumValue = 120; return isOk; }
	if (strcmp(strValue, "searchAttr12") == 0) { enumValue = 121; return isOk; }
	if (strcmp(strValue, "searchAttr13") == 0) { enumValue = 122; return isOk; }
	if (strcmp(strValue, "searchAttr14") == 0) { enumValue = 123; return isOk; }
	if (strcmp(strValue, "searchAttr15") == 0) { enumValue = 124; return isOk; }
	if (strcmp(strValue, "searchAttr16") == 0) { enumValue = 125; return isOk; }
	if (strcmp(strValue, "searchAttr17") == 0) { enumValue = 126; return isOk; }
	if (strcmp(strValue, "searchAttr18") == 0) { enumValue = 127; return isOk; }
	if (strcmp(strValue, "searchAttr19") == 0) { enumValue = 128; return isOk; }
	if (strcmp(strValue, "searchAttr20") == 0) { enumValue = 129; return isOk; }
	if (strcmp(strValue, "searchAttr21") == 0) { enumValue = 130; return isOk; }
	if (strcmp(strValue, "searchAttr22") == 0) { enumValue = 131; return isOk; }
	if (strcmp(strValue, "searchAttr23") == 0) { enumValue = 132; return isOk; }
	if (strcmp(strValue, "searchAttr24") == 0) { enumValue = 133; return isOk; }
	if (strcmp(strValue, "searchAttr25") == 0) { enumValue = 134; return isOk; }
	if (strcmp(strValue, "searchAttr26") == 0) { enumValue = 135; return isOk; }
	if (strcmp(strValue, "searchAttr27") == 0) { enumValue = 136; return isOk; }
	if (strcmp(strValue, "searchAttr28") == 0) { enumValue = 137; return isOk; }
	if (strcmp(strValue, "searchAttr29") == 0) { enumValue = 138; return isOk; }
	if (strcmp(strValue, "searchAttr30") == 0) { enumValue = 139; return isOk; }
	if (strcmp(strValue, "searchAttr31") == 0) { enumValue = 140; return isOk; }
	if (strcmp(strValue, "searchAttr32") == 0) { enumValue = 141; return isOk; }
	if (strcmp(strValue, "searchAttr33") == 0) { enumValue = 142; return isOk; }
	if (strcmp(strValue, "searchAttr34") == 0) { enumValue = 143; return isOk; }
	if (strcmp(strValue, "searchAttr35") == 0) { enumValue = 144; return isOk; }
	if (strcmp(strValue, "searchAttr36") == 0) { enumValue = 145; return isOk; }
	if (strcmp(strValue, "searchAttr37") == 0) { enumValue = 146; return isOk; }
	if (strcmp(strValue, "searchAttr38") == 0) { enumValue = 147; return isOk; }
	if (strcmp(strValue, "searchAttr39") == 0) { enumValue = 148; return isOk; }
	if (strcmp(strValue, "searchAttr40") == 0) { enumValue = 149; return isOk; }
	idaTrackExcept(("StringToEnum::enumOfAttributeId(): invalid parameter \"%s\"",strValue));
	return isNotOk;
}

ReturnStatus StringToEnum::enumOfSearchAttr(ULong& enumValue, const char* strValue)
{
	if (strcmp(strValue, "exactMatch") == 0) { enumValue = 0; return isOk; }
	if (strcmp(strValue, "phonetic1") == 0) { enumValue = 1; return isOk; }
	if (strcmp(strValue, "phonetic2") == 0) { enumValue = 2; return isOk; }
	if (strcmp(strValue, "phonetic3") == 0) { enumValue = 3; return isOk; }
	if (strcmp(strValue, "phonetic4") == 0) { enumValue = 4; return isOk; }
	if (strcmp(strValue, "phoneticMask") == 0) { enumValue = 7; return isOk; }
	if (strcmp(strValue, "equal") == 0) { enumValue = 0; return isOk; }
	if (strcmp(strValue, "greaterThan") == 0) { enumValue = 8; return isOk; }
	if (strcmp(strValue, "lessThan") == 0) { enumValue = 16; return isOk; }
	if (strcmp(strValue, "relationShipMask") == 0) { enumValue = 56; return isOk; }
	if (strcmp(strValue, "wordRotation") == 0) { enumValue = 0; return isOk; }
	if (strcmp(strValue, "noWordRotation") == 0) { enumValue = 64; return isOk; }
	if (strcmp(strValue, "rotationMask") == 0) { enumValue = 64; return isOk; }
	if (strcmp(strValue, "noNullKey") == 0) { enumValue = 0; return isOk; }
	if (strcmp(strValue, "nullKey") == 0) { enumValue = 128; return isOk; }
	if (strcmp(strValue, "nullKeyMask") == 0) { enumValue = 128; return isOk; }
	if (strcmp(strValue, "present") == 0) { enumValue = 0; return isOk; }
	if (strcmp(strValue, "acceptNotPresent") == 0) { enumValue = 256; return isOk; }
	if (strcmp(strValue, "presentMask") == 0) { enumValue = 256; return isOk; }
	if (strcmp(strValue, "noAlias") == 0) { enumValue = 0; return isOk; }
	if (strcmp(strValue, "alias") == 0) { enumValue = 512; return isOk; }
	if (strcmp(strValue, "aliasMask") == 0) { enumValue = 512; return isOk; }
	if (strcmp(strValue, "noInitial") == 0) { enumValue = 0; return isOk; }
	if (strcmp(strValue, "noInitialCharacter") == 0) { enumValue = 0; return isOk; }
	if (strcmp(strValue, "initial") == 0) { enumValue = 1024; return isOk; }
	if (strcmp(strValue, "initialCharacter") == 0) { enumValue = 1024; return isOk; }
	if (strcmp(strValue, "initialMask") == 0) { enumValue = 1024; return isOk; }
	if (strcmp(strValue, "providerDefined") == 0) { enumValue = 0; return isOk; }
	if (strcmp(strValue, "stringExact") == 0) { enumValue = 2048; return isOk; }
	if (strcmp(strValue, "wordDeletion") == 0) { enumValue = 4096; return isOk; }
	if (strcmp(strValue, "wordRestrictedDeletion") == 0) { enumValue = 6144; return isOk; }
	if (strcmp(strValue, "wordRotationSequenceRule") == 0) { enumValue = 8192; return isOk; }
	if (strcmp(strValue, "wordRotationAndDeletion") == 0) { enumValue = 10240; return isOk; }
	if (strcmp(strValue, "sequenceRuleMask") == 0) { enumValue = 30720; return isOk; }
	idaTrackExcept(("StringToEnum::enumOfSearchAttr(): invalid parameter \"%s\"",strValue));
	return isNotOk;
}

ReturnStatus StringToEnum::enumOfProtocol(ULong& enumValue, const char* strValue)
{
	if (strcmp(strValue, "tdfErrorArgProt") == 0) { enumValue = 1; return isOk; }
	if (strcmp(strValue, "idisProt") == 0) { enumValue = 3; return isOk; }
	if (strcmp(strValue, "e115_v0_Prot") == 0) { enumValue = 5; return isOk; }
	if (strcmp(strValue, "e115_v1_Prot") == 0) { enumValue = 6; return isOk; }
	if (strcmp(strValue, "e115_v2_Prot") == 0) { enumValue = 7; return isOk; }
	if (strcmp(strValue, "osa_v2_Prot") == 0) { enumValue = 8; return isOk; }
	if (strcmp(strValue, "tdsProt") == 0) { enumValue = 14; return isOk; }
	if (strcmp(strValue, "tdsDeleteProt") == 0) { enumValue = 15; return isOk; }
	if (strcmp(strValue, "tdsModifyProt") == 0) { enumValue = 16; return isOk; }
	if (strcmp(strValue, "qstProt") == 0) { enumValue = 60; return isOk; }
	if (strcmp(strValue, "ndisProt") == 0) { enumValue = 61; return isOk; }
	if (strcmp(strValue, "humanProt") == 0) { enumValue = 62; return isOk; }
	if (strcmp(strValue, "ldapProt") == 0) { enumValue = 63; return isOk; }
	if (strcmp(strValue, "ddProt") == 0) { enumValue = 64; return isOk; }
	if (strcmp(strValue, "httpProt") == 0) { enumValue = 65; return isOk; }
	if (strcmp(strValue, "osaApiProt") == 0) { enumValue = 66; return isOk; }
	if (strcmp(strValue, "eda_v10_Prot") == 0) { enumValue = 67; return isOk; }
	if (strcmp(strValue, "eda_v11_Prot") == 0) { enumValue = 68; return isOk; }
	if (strcmp(strValue, "ctiProt") == 0) { enumValue = 69; return isOk; }
	if (strcmp(strValue, "voiceProt") == 0) { enumValue = 70; return isOk; }
	if (strcmp(strValue, "f510Prot") == 0) { enumValue = 71; return isOk; }
	if (strcmp(strValue, "smsUcpProt") == 0) { enumValue = 72; return isOk; }
	if (strcmp(strValue, "smsOisProt") == 0) { enumValue = 73; return isOk; }
	if (strcmp(strValue, "smsCimdProt") == 0) { enumValue = 74; return isOk; }
	if (strcmp(strValue, "smsSmppProt") == 0) { enumValue = 75; return isOk; }
	idaTrackExcept(("StringToEnum::enumOfProtocol(): invalid parameter \"%s\"",strValue));
	return isNotOk;
}

ReturnStatus StringToEnum::enumOfSpecialAddress(ULong& enumValue, const char* strValue)
{
	if (strcmp(strValue, "localAddr") == 0) { enumValue = 1; return isOk; }
	if (strcmp(strValue, "humanAddr") == 0) { enumValue = 2; return isOk; }
	if (strcmp(strValue, "unknownAddr") == 0) { enumValue = 3; return isOk; }
	if (strcmp(strValue, "unkownAddr") == 0) { enumValue = 3; return isOk; }
	idaTrackExcept(("StringToEnum::enumOfSpecialAddress(): invalid parameter \"%s\"",strValue));
	return isNotOk;
}

ReturnStatus StringToEnum::enumOfProduct(ULong& enumValue, const char* strValue)
{
	if (strcmp(strValue, "unknownProduct") == 0) { enumValue = 0; return isOk; }
	if (strcmp(strValue, "e115Inbound") == 0) { enumValue = 3; return isOk; }
	if (strcmp(strValue, "trader") == 0) { enumValue = 4; return isOk; }
	if (strcmp(strValue, "idis") == 0) { enumValue = 5; return isOk; }
	if (strcmp(strValue, "qst") == 0) { enumValue = 6; return isOk; }
	if (strcmp(strValue, "ida") == 0) { enumValue = 7; return isOk; }
	if (strcmp(strValue, "ndis") == 0) { enumValue = 9; return isOk; }
	if (strcmp(strValue, "itgOsa") == 0) { enumValue = 40; return isOk; }
	if (strcmp(strValue, "osaInbound") == 0) { enumValue = 40; return isOk; }
	if (strcmp(strValue, "otgOsa") == 0) { enumValue = 45; return isOk; }
	if (strcmp(strValue, "osaOutbound") == 0) { enumValue = 45; return isOk; }
	if (strcmp(strValue, "ddInbound") == 0) { enumValue = 50; return isOk; }
	if (strcmp(strValue, "e115Outbound") == 0) { enumValue = 51; return isOk; }
	if (strcmp(strValue, "edaInbound") == 0) { enumValue = 52; return isOk; }
	if (strcmp(strValue, "edaOutbound") == 0) { enumValue = 53; return isOk; }
	if (strcmp(strValue, "idw") == 0) { enumValue = 54; return isOk; }
	if (strcmp(strValue, "ldapInbound") == 0) { enumValue = 55; return isOk; }
	if (strcmp(strValue, "osaApiCustomerAppl") == 0) { enumValue = 56; return isOk; }
	if (strcmp(strValue, "adis") == 0) { enumValue = 57; return isOk; }
	if (strcmp(strValue, "rdis") == 0) { enumValue = 58; return isOk; }
	if (strcmp(strValue, "mos") == 0) { enumValue = 59; return isOk; }
	if (strcmp(strValue, "adisVoice") == 0) { enumValue = 60; return isOk; }
	if (strcmp(strValue, "adisDtmf") == 0) { enumValue = 61; return isOk; }
	if (strcmp(strValue, "adisWa") == 0) { enumValue = 62; return isOk; }
	if (strcmp(strValue, "adisPg") == 0) { enumValue = 63; return isOk; }
	if (strcmp(strValue, "rdisVoice") == 0) { enumValue = 64; return isOk; }
	if (strcmp(strValue, "rdisDtmf") == 0) { enumValue = 65; return isOk; }
	if (strcmp(strValue, "f510Outbound") == 0) { enumValue = 66; return isOk; }
	if (strcmp(strValue, "f510Inbound") == 0) { enumValue = 67; return isOk; }
	if (strcmp(strValue, "osaTool") == 0) { enumValue = 68; return isOk; }
	if (strcmp(strValue, "spoc") == 0) { enumValue = 69; return isOk; }
	if (strcmp(strValue, "smsGateway") == 0) { enumValue = 70; return isOk; }
	if (strcmp(strValue, "vStation") == 0) { enumValue = 71; return isOk; }
	if (strcmp(strValue, "msgSrv") == 0) { enumValue = 72; return isOk; }
	if (strcmp(strValue, "daInbound") == 0) { enumValue = 80; return isOk; }
	idaTrackExcept(("StringToEnum::enumOfProduct(): invalid parameter \"%s\"",strValue));
	return isNotOk;
}

ReturnStatus StringToEnum::enumOfLanguage(ULong& enumValue, const char* strValue)
{
	if (strcmp(strValue, "languageNeutral") == 0) { enumValue = 0x0000; return isOk; }
	if (strcmp(strValue, "danish") == 0) { enumValue = 0x0406; return isOk; }
	if (strcmp(strValue, "dutch") == 0) { enumValue = 0x0413; return isOk; }
	if (strcmp(strValue, "dutchBelgian") == 0) { enumValue = 0x0813; return isOk; }
	if (strcmp(strValue, "englishAmerican") == 0) { enumValue = 0x0409; return isOk; }
	if (strcmp(strValue, "englishBritish") == 0) { enumValue = 0x0809; return isOk; }
	if (strcmp(strValue, "englishAustralian") == 0) { enumValue = 0x0c09; return isOk; }
	if (strcmp(strValue, "englishCanadian") == 0) { enumValue = 0x1009; return isOk; }
	if (strcmp(strValue, "englishNewZealand") == 0) { enumValue = 0x1409; return isOk; }
	if (strcmp(strValue, "englishIreland") == 0) { enumValue = 0x1809; return isOk; }
	if (strcmp(strValue, "englishSouthAfrica") == 0) { enumValue = 0x1c09; return isOk; }
	if (strcmp(strValue, "finnish") == 0) { enumValue = 0x040b; return isOk; }
	if (strcmp(strValue, "french") == 0) { enumValue = 0x040c; return isOk; }
	if (strcmp(strValue, "frenchBelgian") == 0) { enumValue = 0x080c; return isOk; }
	if (strcmp(strValue, "frenchCanadian") == 0) { enumValue = 0x0c0c; return isOk; }
	if (strcmp(strValue, "frenchSwiss") == 0) { enumValue = 0x100c; return isOk; }
	if (strcmp(strValue, "german") == 0) { enumValue = 0x0407; return isOk; }
	if (strcmp(strValue, "germanSwiss") == 0) { enumValue = 0x0807; return isOk; }
	if (strcmp(strValue, "germanAustrian") == 0) { enumValue = 0x0c07; return isOk; }
	if (strcmp(strValue, "icelandic") == 0) { enumValue = 0x040f; return isOk; }
	if (strcmp(strValue, "italian") == 0) { enumValue = 0x0410; return isOk; }
	if (strcmp(strValue, "italianSwiss") == 0) { enumValue = 0x0810; return isOk; }
	if (strcmp(strValue, "norwegianBokmal") == 0) { enumValue = 0x0414; return isOk; }
	if (strcmp(strValue, "norwegianNynorsk") == 0) { enumValue = 0x0814; return isOk; }
	if (strcmp(strValue, "portugueseBrazilian") == 0) { enumValue = 0x0416; return isOk; }
	if (strcmp(strValue, "portuguese") == 0) { enumValue = 0x0816; return isOk; }
	if (strcmp(strValue, "swedish") == 0) { enumValue = 0x041D; return isOk; }
	if (strcmp(strValue, "swedishFinland") == 0) { enumValue = 0x081D; return isOk; }
	if (strcmp(strValue, "spanish") == 0) { enumValue = 0x040a; return isOk; }
	if (strcmp(strValue, "spanishMexican") == 0) { enumValue = 0x080a; return isOk; }
	if (strcmp(strValue, "spanishModern") == 0) { enumValue = 0x0c0a; return isOk; }
	if (strcmp(strValue, "turkish") == 0) { enumValue = 0x041f; return isOk; }
	if (strcmp(strValue, "polish") == 0) { enumValue = 0x0415; return isOk; }
	if (strcmp(strValue, "czech") == 0) { enumValue = 0x0405; return isOk; }
	if (strcmp(strValue, "slovak") == 0) { enumValue = 0x041b; return isOk; }
	if (strcmp(strValue, "hungarian") == 0) { enumValue = 0x040e; return isOk; }
	if (strcmp(strValue, "russian") == 0) { enumValue = 0x0419; return isOk; }
	if (strcmp(strValue, "greek") == 0) { enumValue = 0x0408; return isOk; }
	if (strcmp(strValue, "thailand") == 0) { enumValue = 0x041e; return isOk; }
	if (strcmp(strValue, "arabicSaudiArabia") == 0) { enumValue = 0x0401; return isOk; }
	if (strcmp(strValue, "arabicEgypt") == 0) { enumValue = 0x0c01; return isOk; }
	if (strcmp(strValue, "slovenia") == 0) { enumValue = 0x0424; return isOk; }
	if (strcmp(strValue, "estonia") == 0) { enumValue = 0x0425; return isOk; }
	if (strcmp(strValue, "latvia") == 0) { enumValue = 0x0426; return isOk; }
	if (strcmp(strValue, "lithuania") == 0) { enumValue = 0x0427; return isOk; }
	if (strcmp(strValue, "croatia") == 0) { enumValue = 0x041A; return isOk; }
	if (strcmp(strValue, "language4belguim") == 0) { enumValue = 0x0601; return isOk; }
	if (strcmp(strValue, "language4liechtenstein") == 0) { enumValue = 0x0602; return isOk; }
	if (strcmp(strValue, "language4luxembourg") == 0) { enumValue = 0x0603; return isOk; }
	if (strcmp(strValue, "language4switzerland") == 0) { enumValue = 0x0604; return isOk; }
	if (strcmp(strValue, "language4monaco") == 0) { enumValue = 0x0605; return isOk; }
	if (strcmp(strValue, "language4usaLocal") == 0) { enumValue = 0x0607; return isOk; }
	if (strcmp(strValue, "language4cypern") == 0) { enumValue = 0x0608; return isOk; }
	if (strcmp(strValue, "language4vaticanState") == 0) { enumValue = 0x0609; return isOk; }
	if (strcmp(strValue, "language4malta") == 0) { enumValue = 0x0610; return isOk; }
	if (strcmp(strValue, "language4sanMarino") == 0) { enumValue = 0x0611; return isOk; }
	if (strcmp(strValue, "language4yugoslavia") == 0) { enumValue = 0x0612; return isOk; }
	idaTrackExcept(("StringToEnum::enumOfLanguage(): invalid parameter \"%s\"",strValue));
	return isNotOk;
}

ReturnStatus StringToEnum::enumOfCharacterSet(ULong& enumValue, const char* strValue)
{
	if (strcmp(strValue, "latin1") == 0) { enumValue = 0; return isOk; }
	if (strcmp(strValue, "ascii") == 0) { enumValue = 1; return isOk; }
	if (strcmp(strValue, "pcplus1") == 0) { enumValue = 2; return isOk; }
	if (strcmp(strValue, "pcplus2") == 0) { enumValue = 3; return isOk; }
	if (strcmp(strValue, "pcplusQst") == 0) { enumValue = 4; return isOk; }
	if (strcmp(strValue, "xlatin1") == 0) { enumValue = 5; return isOk; }
	if (strcmp(strValue, "latin2") == 0) { enumValue = 6; return isOk; }
	if (strcmp(strValue, "cp850") == 0) { enumValue = 7; return isOk; }
	if (strcmp(strValue, "arabic") == 0) { enumValue = 8; return isOk; }
	if (strcmp(strValue, "utf8") == 0) { enumValue = 9; return isOk; }
	if (strcmp(strValue, "e115ascii") == 0) { enumValue = 10; return isOk; }
	if (strcmp(strValue, "cp1250") == 0) { enumValue = 11; return isOk; }
	if (strcmp(strValue, "utf16be") == 0) { enumValue = 20; return isOk; }
	if (strcmp(strValue, "utf16le") == 0) { enumValue = 21; return isOk; }
	if (strcmp(strValue, "utf32be") == 0) { enumValue = 22; return isOk; }
	if (strcmp(strValue, "utf32le") == 0) { enumValue = 23; return isOk; }
	if (strcmp(strValue, "latin3") == 0) { enumValue = 33; return isOk; }
	if (strcmp(strValue, "latin4") == 0) { enumValue = 34; return isOk; }
	if (strcmp(strValue, "latin5") == 0) { enumValue = 35; return isOk; }
	if (strcmp(strValue, "latin9") == 0) { enumValue = 39; return isOk; }
	if (strcmp(strValue, "iso885913") == 0) { enumValue = 43; return isOk; }
	if (strcmp(strValue, "iso88595") == 0) { enumValue = 45; return isOk; }
	if (strcmp(strValue, "iso88596") == 0) { enumValue = 46; return isOk; }
	if (strcmp(strValue, "iso88597") == 0) { enumValue = 47; return isOk; }
	if (strcmp(strValue, "iso88598") == 0) { enumValue = 48; return isOk; }
	if (strcmp(strValue, "cp915") == 0) { enumValue = 50; return isOk; }
	if (strcmp(strValue, "cp942") == 0) { enumValue = 51; return isOk; }
	if (strcmp(strValue, "cp932") == 0) { enumValue = 52; return isOk; }
	if (strcmp(strValue, "cp943") == 0) { enumValue = 53; return isOk; }
	if (strcmp(strValue, "cp950") == 0) { enumValue = 54; return isOk; }
	if (strcmp(strValue, "cp1386") == 0) { enumValue = 55; return isOk; }
	if (strcmp(strValue, "cp1383") == 0) { enumValue = 56; return isOk; }
	if (strcmp(strValue, "cp964") == 0) { enumValue = 57; return isOk; }
	if (strcmp(strValue, "cp949") == 0) { enumValue = 58; return isOk; }
	if (strcmp(strValue, "cp874") == 0) { enumValue = 59; return isOk; }
	if (strcmp(strValue, "cp437") == 0) { enumValue = 60; return isOk; }
	if (strcmp(strValue, "cp851") == 0) { enumValue = 61; return isOk; }
	if (strcmp(strValue, "cp852") == 0) { enumValue = 62; return isOk; }
	if (strcmp(strValue, "cp855") == 0) { enumValue = 63; return isOk; }
	if (strcmp(strValue, "cp856") == 0) { enumValue = 64; return isOk; }
	if (strcmp(strValue, "cp857") == 0) { enumValue = 65; return isOk; }
	if (strcmp(strValue, "cp858") == 0) { enumValue = 66; return isOk; }
	if (strcmp(strValue, "cp860") == 0) { enumValue = 67; return isOk; }
	if (strcmp(strValue, "cp861") == 0) { enumValue = 68; return isOk; }
	if (strcmp(strValue, "cp862") == 0) { enumValue = 69; return isOk; }
	if (strcmp(strValue, "cp863") == 0) { enumValue = 70; return isOk; }
	if (strcmp(strValue, "cp864") == 0) { enumValue = 71; return isOk; }
	if (strcmp(strValue, "cp865") == 0) { enumValue = 72; return isOk; }
	if (strcmp(strValue, "cp866") == 0) { enumValue = 73; return isOk; }
	if (strcmp(strValue, "cp867") == 0) { enumValue = 74; return isOk; }
	if (strcmp(strValue, "cp868") == 0) { enumValue = 75; return isOk; }
	if (strcmp(strValue, "cp869") == 0) { enumValue = 76; return isOk; }
	if (strcmp(strValue, "cp878") == 0) { enumValue = 77; return isOk; }
	if (strcmp(strValue, "cp922") == 0) { enumValue = 78; return isOk; }
	if (strcmp(strValue, "cp1251") == 0) { enumValue = 79; return isOk; }
	if (strcmp(strValue, "cp1252") == 0) { enumValue = 80; return isOk; }
	if (strcmp(strValue, "cp1253") == 0) { enumValue = 81; return isOk; }
	if (strcmp(strValue, "cp1254") == 0) { enumValue = 82; return isOk; }
	if (strcmp(strValue, "cp1255") == 0) { enumValue = 83; return isOk; }
	if (strcmp(strValue, "cp1256") == 0) { enumValue = 84; return isOk; }
	if (strcmp(strValue, "cp1257") == 0) { enumValue = 85; return isOk; }
	if (strcmp(strValue, "cp1258") == 0) { enumValue = 86; return isOk; }
	if (strcmp(strValue, "cp1006") == 0) { enumValue = 87; return isOk; }
	if (strcmp(strValue, "cp1098") == 0) { enumValue = 88; return isOk; }
	if (strcmp(strValue, "big5") == 0) { enumValue = 100; return isOk; }
	if (strcmp(strValue, "big5hk") == 0) { enumValue = 101; return isOk; }
	if (strcmp(strValue, "chinese") == 0) { enumValue = 102; return isOk; }
	if (strcmp(strValue, "korean") == 0) { enumValue = 103; return isOk; }
	if (strcmp(strValue, "macintosh") == 0) { enumValue = 105; return isOk; }
	if (strcmp(strValue, "macgr") == 0) { enumValue = 106; return isOk; }
	if (strcmp(strValue, "maccy") == 0) { enumValue = 107; return isOk; }
	if (strcmp(strValue, "macce") == 0) { enumValue = 108; return isOk; }
	if (strcmp(strValue, "mactr") == 0) { enumValue = 109; return isOk; }
	if (strcmp(strValue, "jis") == 0) { enumValue = 110; return isOk; }
	idaTrackExcept(("StringToEnum::enumOfCharacterSet(): invalid parameter \"%s\"",strValue));
	return isNotOk;
}

ReturnStatus StringToEnum::enumOfSearchType(ULong& enumValue, const char* strValue)
{
	if (strcmp(strValue, "residential") == 0) { enumValue = 10; return isOk; }
	if (strcmp(strValue, "business") == 0) { enumValue = 11; return isOk; }
	if (strcmp(strValue, "government") == 0) { enumValue = 12; return isOk; }
	if (strcmp(strValue, "businessAndGovernment") == 0) { enumValue = 13; return isOk; }
	if (strcmp(strValue, "crossListings") == 0) { enumValue = 14; return isOk; }
	if (strcmp(strValue, "telephoneNumber") == 0) { enumValue = 15; return isOk; }
	if (strcmp(strValue, "city") == 0) { enumValue = 29; return isOk; }
	if (strcmp(strValue, "locality") == 0) { enumValue = 30; return isOk; }
	if (strcmp(strValue, "state") == 0) { enumValue = 31; return isOk; }
	if (strcmp(strValue, "region") == 0) { enumValue = 32; return isOk; }
	if (strcmp(strValue, "book") == 0) { enumValue = 33; return isOk; }
	if (strcmp(strValue, "branch") == 0) { enumValue = 34; return isOk; }
	if (strcmp(strValue, "street") == 0) { enumValue = 35; return isOk; }
	if (strcmp(strValue, "coordinate") == 0) { enumValue = 36; return isOk; }
	if (strcmp(strValue, "list") == 0) { enumValue = 40; return isOk; }
	if (strcmp(strValue, "emergency") == 0) { enumValue = 41; return isOk; }
	if (strcmp(strValue, "translation") == 0) { enumValue = 80; return isOk; }
	idaTrackExcept(("StringToEnum::enumOfSearchType(): invalid parameter \"%s\"",strValue));
	return isNotOk;
}

ReturnStatus StringToEnum::enumOfSearchVar(ULong& enumValue, const char* strValue)
{
	if (strcmp(strValue, "exactMatch") == 0) { enumValue = 0; return isOk; }
	if (strcmp(strValue, "phonetic1") == 0) { enumValue = 1; return isOk; }
	if (strcmp(strValue, "phonetic2") == 0) { enumValue = 2; return isOk; }
	if (strcmp(strValue, "phonetic3") == 0) { enumValue = 3; return isOk; }
	if (strcmp(strValue, "phonetic4") == 0) { enumValue = 4; return isOk; }
	if (strcmp(strValue, "phoneticMask") == 0) { enumValue = 7; return isOk; }
	if (strcmp(strValue, "noModifier") == 0) { enumValue = 0; return isOk; }
	if (strcmp(strValue, "sequential") == 0) { enumValue = 8; return isOk; }
	if (strcmp(strValue, "unlimited") == 0) { enumValue = 16; return isOk; }
	if (strcmp(strValue, "modifierMask") == 0) { enumValue = 24; return isOk; }
	if (strcmp(strValue, "noDemerit") == 0) { enumValue = 0; return isOk; }
	if (strcmp(strValue, "demerit") == 0) { enumValue = 32; return isOk; }
	if (strcmp(strValue, "demeritMask") == 0) { enumValue = 32; return isOk; }
	if (strcmp(strValue, "wordRotation") == 0) { enumValue = 0; return isOk; }
	if (strcmp(strValue, "noWordRotation") == 0) { enumValue = 64; return isOk; }
	if (strcmp(strValue, "rotationMask") == 0) { enumValue = 64; return isOk; }
	if (strcmp(strValue, "dontRepeatHeadings") == 0) { enumValue = 0; return isOk; }
	if (strcmp(strValue, "repeatHeadings") == 0) { enumValue = 128; return isOk; }
	if (strcmp(strValue, "headingsMask") == 0) { enumValue = 128; return isOk; }
	if (strcmp(strValue, "dontHideCaptions") == 0) { enumValue = 0; return isOk; }
	if (strcmp(strValue, "hideCaptions") == 0) { enumValue = 256; return isOk; }
	if (strcmp(strValue, "captionsMask") == 0) { enumValue = 256; return isOk; }
	if (strcmp(strValue, "noAlias") == 0) { enumValue = 0; return isOk; }
	if (strcmp(strValue, "alias") == 0) { enumValue = 512; return isOk; }
	if (strcmp(strValue, "aliasMask") == 0) { enumValue = 512; return isOk; }
	if (strcmp(strValue, "noLowIndent") == 0) { enumValue = 0; return isOk; }
	if (strcmp(strValue, "lowIndent") == 0) { enumValue = 1024; return isOk; }
	if (strcmp(strValue, "highIndent") == 0) { enumValue = 4194304; return isOk; }
	if (strcmp(strValue, "indentMask") == 0) { enumValue = 4195328; return isOk; }
	if (strcmp(strValue, "noEmptyStreet") == 0) { enumValue = 0; return isOk; }
	if (strcmp(strValue, "emptyStreet") == 0) { enumValue = 2048; return isOk; }
	if (strcmp(strValue, "streetTypeMask") == 0) { enumValue = 2048; return isOk; }
	if (strcmp(strValue, "noRecordTypeReturn") == 0) { enumValue = 0; return isOk; }
	if (strcmp(strValue, "recordTypeReturn") == 0) { enumValue = 4096; return isOk; }
	if (strcmp(strValue, "returnRecordTypeMask") == 0) { enumValue = 4096; return isOk; }
	if (strcmp(strValue, "noRange") == 0) { enumValue = 0; return isOk; }
	if (strcmp(strValue, "range") == 0) { enumValue = 8192; return isOk; }
	if (strcmp(strValue, "zipTypeMask") == 0) { enumValue = 8192; return isOk; }
	if (strcmp(strValue, "twoStep") == 0) { enumValue = 0; return isOk; }
	if (strcmp(strValue, "oneStep") == 0) { enumValue = 16384; return isOk; }
	if (strcmp(strValue, "filteringMask") == 0) { enumValue = 16384; return isOk; }
	if (strcmp(strValue, "noLocalityFilter") == 0) { enumValue = 0; return isOk; }
	if (strcmp(strValue, "localityFilter") == 0) { enumValue = 32768; return isOk; }
	if (strcmp(strValue, "localityFilterMask") == 0) { enumValue = 32768; return isOk; }
	if (strcmp(strValue, "dontUseShowLoc") == 0) { enumValue = 0; return isOk; }
	if (strcmp(strValue, "useShowLoc") == 0) { enumValue = 65536; return isOk; }
	if (strcmp(strValue, "showLocMask") == 0) { enumValue = 65536; return isOk; }
	if (strcmp(strValue, "disableImplicitBranchList") == 0) { enumValue = 0; return isOk; }
	if (strcmp(strValue, "enableImplicitBranchList") == 0) { enumValue = 131072; return isOk; }
	if (strcmp(strValue, "implicitBranchListMask") == 0) { enumValue = 131072; return isOk; }
	if (strcmp(strValue, "noBranchFilter") == 0) { enumValue = 0; return isOk; }
	if (strcmp(strValue, "branchFilter") == 0) { enumValue = 262144; return isOk; }
	if (strcmp(strValue, "branchFilterMask") == 0) { enumValue = 262144; return isOk; }
	if (strcmp(strValue, "noVoice") == 0) { enumValue = 0; return isOk; }
	if (strcmp(strValue, "voice") == 0) { enumValue = 524288; return isOk; }
	if (strcmp(strValue, "voiceMask") == 0) { enumValue = 524288; return isOk; }
	if (strcmp(strValue, "disableImplicitStreetList") == 0) { enumValue = 0; return isOk; }
	if (strcmp(strValue, "enableImplicitStreetList") == 0) { enumValue = 1048576; return isOk; }
	if (strcmp(strValue, "implicitStreetListMask") == 0) { enumValue = 1048576; return isOk; }
	if (strcmp(strValue, "noStreetFilter") == 0) { enumValue = 0; return isOk; }
	if (strcmp(strValue, "streetFilter") == 0) { enumValue = 2097152; return isOk; }
	if (strcmp(strValue, "streetFilterMask") == 0) { enumValue = 2097152; return isOk; }
	idaTrackExcept(("StringToEnum::enumOfSearchVar(): invalid parameter \"%s\"",strValue));
	return isNotOk;
}

ReturnStatus StringToEnum::enumOfExpansion(ULong& enumValue, const char* strValue)
{
	if (strcmp(strValue, "exactLocality") == 0) { enumValue = 0; return isOk; }
	if (strcmp(strValue, "expandedLocality") == 0) { enumValue = 1; return isOk; }
	if (strcmp(strValue, "limitedExpandedLocality") == 0) { enumValue = 2; return isOk; }
	if (strcmp(strValue, "stateOrProvince") == 0) { enumValue = 3; return isOk; }
	if (strcmp(strValue, "regionRange") == 0) { enumValue = 4; return isOk; }
	if (strcmp(strValue, "countryRange") == 0) { enumValue = 5; return isOk; }
	if (strcmp(strValue, "expandedSubscriber") == 0) { enumValue = 6; return isOk; }
	if (strcmp(strValue, "localityEqual") == 0) { enumValue = 7; return isOk; }
	if (strcmp(strValue, "houseNumberRange") == 0) { enumValue = 8; return isOk; }
	if (strcmp(strValue, "cityRange") == 0) { enumValue = 9; return isOk; }
	if (strcmp(strValue, "bookRange") == 0) { enumValue = 10; return isOk; }
	if (strcmp(strValue, "exactDbu") == 0) { enumValue = 32; return isOk; }
	if (strcmp(strValue, "dbuEqual") == 0) { enumValue = 39; return isOk; }
	idaTrackExcept(("StringToEnum::enumOfExpansion(): invalid parameter \"%s\"",strValue));
	return isNotOk;
}

ReturnStatus StringToEnum::enumOfRequestedFormat(ULong& enumValue, const char* strValue)
{
	if (strcmp(strValue, "qstScreenFormat") == 0) { enumValue = 1; return isOk; }
	if (strcmp(strValue, "indentedRecordFormat") == 0) { enumValue = 2; return isOk; }
	idaTrackExcept(("StringToEnum::enumOfRequestedFormat(): invalid parameter \"%s\"",strValue));
	return isNotOk;
}

ReturnStatus StringToEnum::enumOfDbAttribute(ULong& enumValue, const char* strValue)
{
	if (strcmp(strValue, "name") == 0) { enumValue = 1; return isOk; }
	if (strcmp(strValue, "givenName") == 0) { enumValue = 2; return isOk; }
	if (strcmp(strValue, "streetName") == 0) { enumValue = 3; return isOk; }
	if (strcmp(strValue, "houseNumber") == 0) { enumValue = 4; return isOk; }
	if (strcmp(strValue, "maidenName") == 0) { enumValue = 5; return isOk; }
	if (strcmp(strValue, "subordinateSearchInformation") == 0) { enumValue = 6; return isOk; }
	if (strcmp(strValue, "businessCategoryOrProfession") == 0) { enumValue = 7; return isOk; }
	if (strcmp(strValue, "addressText") == 0) { enumValue = 8; return isOk; }
	if (strcmp(strValue, "zipCode") == 0) { enumValue = 9; return isOk; }
	if (strcmp(strValue, "localityName") == 0) { enumValue = 10; return isOk; }
	if (strcmp(strValue, "telephoneNumberDirectiveText") == 0) { enumValue = 11; return isOk; }
	if (strcmp(strValue, "areaCode") == 0) { enumValue = 12; return isOk; }
	if (strcmp(strValue, "telephoneNumber") == 0) { enumValue = 13; return isOk; }
	if (strcmp(strValue, "telnoMask") == 0) { enumValue = 14; return isOk; }
	if (strcmp(strValue, "faxNumber") == 0) { enumValue = 15; return isOk; }
	if (strcmp(strValue, "eMailAddress") == 0) { enumValue = 16; return isOk; }
	if (strcmp(strValue, "profession") == 0) { enumValue = 17; return isOk; }
	if (strcmp(strValue, "quarterName") == 0) { enumValue = 18; return isOk; }
	if (strcmp(strValue, "extension") == 0) { enumValue = 20; return isOk; }
	if (strcmp(strValue, "billingInformation") == 0) { enumValue = 21; return isOk; }
	if (strcmp(strValue, "stateName") == 0) { enumValue = 22; return isOk; }
	if (strcmp(strValue, "regionName") == 0) { enumValue = 23; return isOk; }
	if (strcmp(strValue, "countryName") == 0) { enumValue = 24; return isOk; }
	if (strcmp(strValue, "subscriberInfo1") == 0) { enumValue = 25; return isOk; }
	if (strcmp(strValue, "subscriberInfo2") == 0) { enumValue = 26; return isOk; }
	if (strcmp(strValue, "subscriberInfo3") == 0) { enumValue = 27; return isOk; }
	if (strcmp(strValue, "subscriberInfo4") == 0) { enumValue = 28; return isOk; }
	if (strcmp(strValue, "ypSupAddress") == 0) { enumValue = 29; return isOk; }
	if (strcmp(strValue, "ypSubAddress") == 0) { enumValue = 29; return isOk; }
	if (strcmp(strValue, "dataProvider") == 0) { enumValue = 30; return isOk; }
	if (strcmp(strValue, "localityInfo1") == 0) { enumValue = 31; return isOk; }
	if (strcmp(strValue, "localityInfo2") == 0) { enumValue = 32; return isOk; }
	if (strcmp(strValue, "localityInfo3") == 0) { enumValue = 33; return isOk; }
	if (strcmp(strValue, "localityInfo4") == 0) { enumValue = 34; return isOk; }
	if (strcmp(strValue, "subscriberInfo5") == 0) { enumValue = 35; return isOk; }
	if (strcmp(strValue, "subscriberInfo6") == 0) { enumValue = 36; return isOk; }
	if (strcmp(strValue, "localityCode") == 0) { enumValue = 37; return isOk; }
	if (strcmp(strValue, "bookName") == 0) { enumValue = 38; return isOk; }
	if (strcmp(strValue, "cityName") == 0) { enumValue = 39; return isOk; }
	if (strcmp(strValue, "url") == 0) { enumValue = 40; return isOk; }
	if (strcmp(strValue, "nameText") == 0) { enumValue = 41; return isOk; }
	if (strcmp(strValue, "dsaSubAddress") == 0) { enumValue = 42; return isOk; }
	if (strcmp(strValue, "coordinateX") == 0) { enumValue = 43; return isOk; }
	if (strcmp(strValue, "subscriberCoordsX") == 0) { enumValue = 43; return isOk; }
	if (strcmp(strValue, "coordinateY") == 0) { enumValue = 44; return isOk; }
	if (strcmp(strValue, "subscriberCoordsY") == 0) { enumValue = 44; return isOk; }
	if (strcmp(strValue, "subscriberInfo7") == 0) { enumValue = 45; return isOk; }
	if (strcmp(strValue, "subscriberInfo8") == 0) { enumValue = 46; return isOk; }
	if (strcmp(strValue, "subscriberInfo9") == 0) { enumValue = 47; return isOk; }
	if (strcmp(strValue, "subscriberInfo10") == 0) { enumValue = 48; return isOk; }
	if (strcmp(strValue, "subscriberInfo11") == 0) { enumValue = 49; return isOk; }
	if (strcmp(strValue, "subscriberInfo12") == 0) { enumValue = 50; return isOk; }
	if (strcmp(strValue, "subscriberInfo13") == 0) { enumValue = 51; return isOk; }
	if (strcmp(strValue, "subscriberInfo14") == 0) { enumValue = 52; return isOk; }
	if (strcmp(strValue, "subscriberInfo15") == 0) { enumValue = 53; return isOk; }
	if (strcmp(strValue, "countryCode") == 0) { enumValue = 54; return isOk; }
	if (strcmp(strValue, "directoryType") == 0) { enumValue = 56; return isOk; }
	if (strcmp(strValue, "transSource") == 0) { enumValue = 64; return isOk; }
	if (strcmp(strValue, "transDest") == 0) { enumValue = 65; return isOk; }
	if (strcmp(strValue, "topic") == 0) { enumValue = 68; return isOk; }
	if (strcmp(strValue, "shortcut") == 0) { enumValue = 69; return isOk; }
	if (strcmp(strValue, "text") == 0) { enumValue = 70; return isOk; }
	if (strcmp(strValue, "information") == 0) { enumValue = 71; return isOk; }
	if (strcmp(strValue, "dbuName") == 0) { enumValue = 72; return isOk; }
	if (strcmp(strValue, "noOfRecords") == 0) { enumValue = 73; return isOk; }
	if (strcmp(strValue, "dbuVersion") == 0) { enumValue = 74; return isOk; }
	if (strcmp(strValue, "distance") == 0) { enumValue = 100; return isOk; }
	if (strcmp(strValue, "subscriberInfo16") == 0) { enumValue = 101; return isOk; }
	if (strcmp(strValue, "subscriberInfo17") == 0) { enumValue = 102; return isOk; }
	if (strcmp(strValue, "subscriberInfo18") == 0) { enumValue = 103; return isOk; }
	if (strcmp(strValue, "subscriberInfo19") == 0) { enumValue = 104; return isOk; }
	if (strcmp(strValue, "subscriberInfo20") == 0) { enumValue = 105; return isOk; }
	if (strcmp(strValue, "direction") == 0) { enumValue = 106; return isOk; }
	if (strcmp(strValue, "subscriberInfo21") == 0) { enumValue = 160; return isOk; }
	if (strcmp(strValue, "subscriberInfo22") == 0) { enumValue = 161; return isOk; }
	if (strcmp(strValue, "subscriberInfo23") == 0) { enumValue = 162; return isOk; }
	if (strcmp(strValue, "subscriberInfo24") == 0) { enumValue = 163; return isOk; }
	if (strcmp(strValue, "subscriberInfo25") == 0) { enumValue = 164; return isOk; }
	if (strcmp(strValue, "subscriberInfo26") == 0) { enumValue = 165; return isOk; }
	if (strcmp(strValue, "subscriberInfo27") == 0) { enumValue = 166; return isOk; }
	if (strcmp(strValue, "subscriberInfo28") == 0) { enumValue = 167; return isOk; }
	if (strcmp(strValue, "subscriberInfo29") == 0) { enumValue = 168; return isOk; }
	if (strcmp(strValue, "subscriberInfo30") == 0) { enumValue = 169; return isOk; }
	if (strcmp(strValue, "subscriberInfo31") == 0) { enumValue = 170; return isOk; }
	if (strcmp(strValue, "subscriberInfo32") == 0) { enumValue = 171; return isOk; }
	if (strcmp(strValue, "subscriberInfo33") == 0) { enumValue = 172; return isOk; }
	if (strcmp(strValue, "subscriberInfo34") == 0) { enumValue = 173; return isOk; }
	if (strcmp(strValue, "subscriberInfo35") == 0) { enumValue = 174; return isOk; }
	if (strcmp(strValue, "subscriberInfo36") == 0) { enumValue = 175; return isOk; }
	if (strcmp(strValue, "subscriberInfo37") == 0) { enumValue = 176; return isOk; }
	if (strcmp(strValue, "subscriberInfo38") == 0) { enumValue = 177; return isOk; }
	if (strcmp(strValue, "subscriberInfo39") == 0) { enumValue = 178; return isOk; }
	if (strcmp(strValue, "subscriberInfo40") == 0) { enumValue = 179; return isOk; }
	idaTrackExcept(("StringToEnum::enumOfDbAttribute(): invalid parameter \"%s\"",strValue));
	return isNotOk;
}


// *** EOF ***

