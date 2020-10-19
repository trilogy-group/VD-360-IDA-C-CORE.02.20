#!/usr/local/bin/perl

################################################################################
#
#  This Perl-Script generates the following files:
#
#     StringToEnum.h          ( in the include path )
#     StringToEnum.cc         ( in the actual path )
#
#
#  created by lua Jan 2001
#
################################################################################


#  -----------------------------------------------------------------------------
#                  S t a r t  o f  c o n f i g u r a t i o n
#  -----------------------------------------------------------------------------

#  Modul Name (= class name)
$MODULNAME = "StringToEnum";
$FILE_BASENAME = "IdaStringToEnum";

#  Header filename
$HFileName = $FILE_BASENAME . ".h";

#  Implementation filename
$CCFileName = $FILE_BASENAME . ".cc";

#  OSA Path: The full path of all OSA include files without trailing slash
#            This may vary depending on the machine (HPUX / AIX) you are working with
# OSAAPIHOME=/pool/osaapi/OSA_API.$(OSAAPIVER)/classlib.$(CLVER)
$OSA_PATH = $ENV{"OSAAPIHOME"} . "/include";
print "using OSA API: $OSA_PATH\n";

#  relative include path without trailing slash
$INCLUDE = "../TdfProcess";

#  In this area please enter a list parameter.
#  Each line contains the include-file name, the name of the enum declaration,
#  the new functionname und an indicator which forces an enum-join or not
push @ENUMS, ('searchspec.h',   'Operation',          'Operation',       '.');
push @ENUMS, ('modifyrequest.h','Operation',          'ModifyOperation', '.');

push @ENUMS, ('searchattr.h',   'AttributeId',        'AttributeId',     '.');

push @ENUMS, ('searchattr.h',   'Phonetic',           'SearchAttr',      '+');
push @ENUMS, ('searchattr.h',   'RelationShip',       'SearchAttr',      '+');
push @ENUMS, ('searchattr.h',   'WordRotation',       'SearchAttr',      '+');
push @ENUMS, ('searchattr.h',   'NullKey',            'SearchAttr',      '+');
push @ENUMS, ('searchattr.h',   'Present',            'SearchAttr',      '+');
push @ENUMS, ('searchattr.h',   'Alias',              'SearchAttr',      '+');
push @ENUMS, ('searchattr.h',   'Initial',            'SearchAttr',      '+');
push @ENUMS, ('searchattr.h',   'SequenceRule',       'SearchAttr',      '.');

push @ENUMS, ('osaapidefs.h',   'Protocol',           'Protocol',        '.');
push @ENUMS, ('osaapidefs.h',   'SpecialAddress',     'SpecialAddress',  '.');
push @ENUMS, ('osaapidefs.h',   'Product',            'Product',         '.');
push @ENUMS, ('osaapidefs.h',   'Language',           'Language',        '.');
push @ENUMS, ('osaapidefs.h',   'CharacterSet',       'CharacterSet',    '.');

push @ENUMS, ('searchvar.h',    'SearchType',         'SearchType',      '.');

push @ENUMS, ('searchvar.h',    'Phonetic',           'SearchVar',       '+');
push @ENUMS, ('searchvar.h',    'Modifier',           'SearchVar',       '+');
push @ENUMS, ('searchvar.h',    'Demerit',            'SearchVar',       '+');
push @ENUMS, ('searchvar.h',    'WordRotation',       'SearchVar',       '+');
push @ENUMS, ('searchvar.h',    'Headings',           'SearchVar',       '+');
push @ENUMS, ('searchvar.h',    'Captions',           'SearchVar',       '+');
push @ENUMS, ('searchvar.h',    'Alias',              'SearchVar',       '+');
push @ENUMS, ('searchvar.h',    'Indent',             'SearchVar',       '+');
push @ENUMS, ('searchvar.h',    'Street',             'SearchVar',       '+');
push @ENUMS, ('searchvar.h',    'ReturnRecordType',   'SearchVar',       '+');
push @ENUMS, ('searchvar.h',    'ZipType',            'SearchVar',       '+');
push @ENUMS, ('searchvar.h',    'Filtering',          'SearchVar',       '+');
push @ENUMS, ('searchvar.h',    'LocalityFilter',     'SearchVar',       '+');
push @ENUMS, ('searchvar.h',    'ShowLoc',            'SearchVar',       '+');
push @ENUMS, ('searchvar.h',    'ImplicitBranchList', 'SearchVar',       '+');
push @ENUMS, ('searchvar.h',    'BranchFilter',       'SearchVar',       '+');
push @ENUMS, ('searchvar.h',    'Voice',              'SearchVar',       '+');
push @ENUMS, ('searchvar.h',    'ImplicitStreetList', 'SearchVar',       '+');
push @ENUMS, ('searchvar.h',    'StreetFilter',       'SearchVar',       '.');

push @ENUMS, ('searchvar.h',    'Expansion',          'Expansion',       '.');

push @ENUMS, ('requestedres.h', 'RequestedFormat',    'RequestedFormat', '.');

push @ENUMS, ('dbattribute.h',  'AttributeId',        'DbAttribute',     '.');




#  -----------------------------------------------------------------------------
#                    E n d   o f   c o n f i g u r a t i o n
#  -----------------------------------------------------------------------------






################################################################################
#
#  main program
#

# ------------------------------------------------------------------------------
#  open outputfiles

print "creating \"$INCLUDE/$HFileName\" ...\n";
open H_FILE, ">$INCLUDE/$HFileName"
   or die "ERROR: Could not open \"$HFileName\" for output !\n";

print "creating \"$CCFileName\" ...\n";
open CC_FILE, ">$CCFileName "
   or die "ERROR: Could not open \"$CCFileName\" for output !\n";

print "\n";

# ------------------------------------------------------------------------------
#  create the file headers

print_H_FILE_Header();
print_CC_FILE_Header();

# ------------------------------------------------------------------------------
#  iteration over all enum declarations

$OldFctName = "";
$i = 0;
while ($ENUMS[$i])
{
   # parameters: filename, enum-name, function-name
   convEnum($ENUMS[$i], $ENUMS[$i+1], $ENUMS[$i+2], $ENUMS[$i+3]);
   $i += 4;
}

# ------------------------------------------------------------------------------
#  complete the c-syntax of all outputfiles

print_H_FILE_Footer();
print_CC_FILE_Footer();

# ------------------------------------------------------------------------------
#  close outputfiles

close H_FILE;
close CC_FILE;

# *** end main ***



################################################################################
#
#  This function is called on every enum declaration
#
sub convEnum
{
   $filename = shift;      # get parameter 1
   $enumname = shift;      # get parameter 2
   $fctname  = shift;      # get parameter 3
   $append   = shift;	   # get parameter 4;

   print "processing \"$filename\": enum $enumname ... ";

   if (open FILE, $OSA_PATH . '/' . $filename)
   {
      # ------------------------------------------------------------------------
      # create
      if ($fctname ne $OldFctName)
      {
		  print H_FILE "\tstatic ReturnStatus enumOf$fctname(ULong& enumValue, const char* strValue);\n";

		  print CC_FILE "ReturnStatus $MODULNAME\:\:enumOf$fctname(ULong& enumValue, const char* strValue)\n";
		  print CC_FILE "{\n";
      }

      # ------------------------------------------------------------------------
      # iteration over all lines of the actual file
      while (<FILE>)
      {
         if (/^\s*enum\s+(\w+)\s+/ && $1 eq $enumname)
         {
            while (<FILE>)
            {
               last if /\}/;     # this stops the iteration when we arrive a '}'
               if (/^\s*(\w+)\s*=\s*(0x[0-9a-zA-Z]+|\d+)/)
               {
                  print CC_FILE "\tif (strcmp(strValue, \"$1\") == 0) { enumValue = $2; return isOk; }\n";
               }
            }
         }
      }
		
	  if ($append eq '.')
	  {
		  print CC_FILE "\tidaTrackExcept((\"StringToEnum::enumOf$fctname(): invalid parameter \\\"%s\\\"\",strValue));\n";
		  print CC_FILE "\treturn isNotOk;\n";
		  print CC_FILE "}\n\n";
	  }

	  $OldFctName = $fctname;

      close FILE;
   }
   else
   {
      print "\ncould not open file $OSA_PATH / $filename \n"; 
   }

   print "ok.\n";
}


################################################################################
#
#  H-File template
#
sub print_H_FILE_Header
{
print H_FILE <<EOT

















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
//                  TdfProcess\gencode.pl 1.0 12-APR-2008 18:52:13 DMSYS
//
//   File:      TdfProcess\gencode.pl
//   Revision:      1.0
//   Date:          12-APR-2008 18:52:13
//
//   DESCRIPTION:
//     
//     This modul converts a string into the int value of the enum declaration
//     with the same identifier
//
//
//<CE---------------------------------------------------------------------------

static const char * SCCS_Id_IdaStringToEnum_h = "@(#) TdfProcess\gencode.pl 1.0 12-APR-2008 18:52:13 DMSYS";


// You can not instanciate this class

class StringToEnum
{
   private:
      StringToEnum();
      ~StringToEnum();

   public:
   
      enum __ret_value
	  {
	     invalid = 0xFFFFFFFF
      };

EOT
;
}
################################################################################
sub print_H_FILE_Footer
{
print H_FILE <<EOT
};

 
// *** EOF ***

EOT
;
}


################################################################################
#
#  CC-File template
#
sub print_CC_FILE_Header
{
print CC_FILE <<EOT

















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
//                  TdfProcess\gencode.pl 1.0 12-APR-2008 18:52:13 DMSYS
//
//   File:      TdfProcess\gencode.pl
//   Revision:      1.0
//   Date:          12-APR-2008 18:52:13
//
//   DESCRIPTION:
//     
//     
//
//
//<CE---------------------------------------------------------------------------

static const char * SCCS_Id_IdaStringToEnum_cc = "@(#) TdfProcess\gencode.pl 1.0 12-APR-2008 18:52:13 DMSYS";


#include <string.h>
#include <pcpdefs.h>
#include <IdaStringToEnum.h>
#include <idatraceman.h>


EOT
;
}
################################################################################
sub print_CC_FILE_Footer
{
print CC_FILE <<EOT

// *** EOF ***

EOT
;
}




# *** EOF ***



