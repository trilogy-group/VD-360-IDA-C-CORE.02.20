#ifndef IdaXmlTags_h
#define IdaXmlTags_h
//CB>-------------------------------------------------------------------
//
//   File, Component, Release:
//                  IdaXmlTags.h 1.1
//
//   File:      IdaXmlTags.h
//   Revision:  1.1
//   Date:      09-JAN-2009 09:42:57
//
//   DESCRIPTION:
//
//
//
//<CE-------------------------------------------------------------------


static const char * SCCS_Id_IdaXmlTags_h = "@(#) IdaXmlTags.h 1.1";


//	Für die Erzeugung eines XML-Dokumentes:
//	-------------------------------------------------------------
//	Die folgenden Konstanten werden hier vordefiniert um bei der
//	String-Konkatenation maximale Performance zu erreichen.
//	Die Vorgehensweise ist notwendig, da die "cat"-Funktion der
//	String-Klasse nur für die Argument-Typen "char" und "String"
//	definiert ist, nicht aber für "char*". Bei Verwendung von
//	"char*" würde der Konstruktor von String jedesmal implizit
//	aufgerufen werden.




static const String tagHEADER("<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n");

static const String tagStart("<");
static const String tagcStart("</");
static const String tagEnd(">");
static const String tagEndNL(">\n");
static const String tagLT("&lt;");
static const String tagGT("&gt;");
static const String tagAMP("&amp;");
static const String tagAPOS("&apos;");
static const String tagQUOT("&quot;");

static const String tagoOsaResponse("<OsaResponse");
static const String tagcOsaResponse("</OsaResponse>\n");
static const String tagoTdsResponse("<TdsResponse>\n");
static const String tagcTdsResponse("</TdsResponse>\n");
static const String tagoSearchResult("<SearchResult>\n");
static const String tagcSearchResult("</SearchResult>\n");
static const String tagoSearchSpec("<SearchSpec>\n");
static const String tagcSearchSpec("</SearchSpec>\n");
static const String tagoRequestedRes("<RequestedRes>\n");
static const String tagcRequestedRes("</RequestedRes>\n");
static const String tagoResultData("<ResultData>\n");
static const String tagcResultData("</ResultData>\n");
static const String tagoVectorOfIndentRecord("<VectorOfIndentRecord>\n");
static const String tagcVectorOfIndentRecord("</VectorOfIndentRecord>\n");
static const String tagoVectorOfLinkContext("<VectorOfLinkContext>\n");
static const String tagcVectorOfLinkContext("</VectorOfLinkContext>\n");
static const String tagoSetOfAttributeId("<SetOfAttributeId>\n");
static const String tagcSetOfAttributeId("</SetOfAttributeId>\n");
static const String tagoListOfRecords("<ListOfRecords>\n");
static const String tagcListOfRecords("</ListOfRecords>\n");
static const String tagoIndentRecord("<IndentRecord>\n");
static const String tagcIndentRecord("</IndentRecord>\n");
static const String tagoRecord("<Record>\n");
static const String tagcRecord("</Record>\n");
static const String tagoSetOfDbAttribute("<SetOfDbAttribute>\n");
static const String tagcSetOfDbAttribute("</SetOfDbAttribute>\n");
static const String tagoDbAttribute("<DbAttribute>\n");
static const String tagcDbAttribute("</DbAttribute>\n");
static const String tagoError("<Error>\n");
static const String tagcError("</Error>\n");
static const String tagoChannel("<Channel>");
static const String tagcChannel("</Channel>");

static const String tagApplicationId("ApplicationId");
static const String tagAttributeId("AttributeId");
static const String tagChannel("Channel");
static const String tagClass("Class");
static const String tagColumns("Columns");
static const String tagData("Data");
static const String tagDataType("DataType");
static const String tagDetail("Detail");
static const String tagErrorAttribute("ErrorAttribute");
static const String tagErrorCode("ErrorCode");
static const String tagErrorSource("ErrorSource");
static const String tagErrorText("ErrorText");
static const String tagErrorWord("ErrorWord");
static const String tagExpRecordLines("ExpRecordLines");
static const String tagFlag("Flag");
static const String tagFlags("Flags");
static const String tagFormatId("FormatId");
static const String tagIndentLevel("IndentLevel");
static const String tagLines("Lines");
static const String tagLinkContext("LinkContext");
static const String tagMaxRecords("MaxRecords");
static const String tagNumberOfMarkedRecordsReturned("NumberOfMarkedRecordsReturned");
static const String tagNumberOfRecordsReturned("NumberOfRecordsReturned");
static const String tagOldRecordType("OldRecordType");
static const String tagOperation("Operation");
static const String tagOrdering("Ordering");
static const String tagRecordId("RecordId");
static const String tagRecordSource("RecordSource");
static const String tagRecordType("RecordType");
static const String tagRecordVersion("RecordVersion");
static const String tagRequestedFormat("RequestedFormat");
static const String tagResultSource("ResultSource");
static const String tagResultSpecifier("ResultSpecifier");
static const String tagSearchRef("SearchRef");
static const String tagSearchRefForChannel("SearchRefForChannel");
static const String tagSource("Source");
static const String tagSourceId("SourceId");
static const String tagSourceLanguage("SourceLanguage");
static const String tagTdfLinkContext("TdfLinkContext");
static const String tagText("Text");
static const String tagTotalNumberOfMarkedRecords("TotalNumberOfMarkedRecords");
static const String tagTotalNumberOfRecords("TotalNumberOfRecords");
static const String tagUsedCharacterSet("OsaCharacterSet");		// geändert 26.3.2001 (lua)
static const String tagWidth("Width");


#endif	// IdaXmlTags_h



// *** EOF ***



