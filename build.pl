#!/usr/bin/perl
########################################################################
#  Usage:
#
#    build.pl 
#
#  Arguments:
#
#    [help] | [[bi=<bif>|clean|check|compile|purify|optimize|debug|profiling|all|ignore|killall]]
#    Look subroutine showHelp for further details
#
########################################################################
#  Description:
#
#    This is the build script for the classlib ZMR libraries, binaries
#    and modules.
#
########################################################################
#
#    Version date:     Item A148078.A
#
########################################################################
#
################ main program ############################
#

use XML::Simple; # qw(:strict);
use Data::Dumper;

  
initialize();
setLogFileName();

getArguments();

my $pool;
my $scriptpool;
my $scriptExt;

if ($UNAME ne "Windows")
  {
    $pool="/pool";
    $scriptpool="/scriptpool";
    $scriptext="ksh";
  }
else
  {
    $pool="//obelix/winpool/pool";
    $scriptpool="//schwarz/scriptpool";
    $scriptext="bat";
    $XML::Simple::PREFERRED_PARSER="XML::Parser";
    use XML::Parser;
  }

if ( $buildInfoFile ne "ignore" )
  {
    readBuildInfo ();
  }

if ($kill eq "yes")
  {
     if ($UNAME ne "Windows")
       {
	  print "Killing all platform directories...\n";
       }
     else
       {
	  print "Maybe the Unix directories in the root level can't be deleted completely\nbecause of sybmbolic links created in Unix!\n";
	  print "Trying to kill all platform directories...\n";	  
       }
     
     killAll();
     exit;
  }

if ($check eq  "yes") 
  {
     checkAll();
     exit;
  }

prepareLogFile();

if ($clean eq "yes")
  {
     doExecuteAll("clean ");
   }
else
  {
    print "clean before building to make sure all objrcts have been built on this os :-)\n";
  }

if ($compile eq "no")
  {
     exit;
  }

createStructure();


if ($compile eq "yes")
  {
     doExecuteAll();
     checkAll();
  }


#
# End of main program
#


sub initialize
  {
	  $logfile="";
     $err=0;
     @tmplist=();
     @SUPPORTED_PLATFORMS=qw (AIX52 HP-UX11 SuSE-Linux9 SuSE-Linux10 Windows2000);

	 ################ determine platform  #######################
     #
     
     $UNAME=`uname | sed -e 's/^CYGWIN.*/CYGWIN/'`;
     chomp $UNAME;
     
     
     #
     ################ determine machine name #######################
     #
	  # Remark: The file lists depend on the OS!
     if ($UNAME eq "AIX")
       {
	  $HOSTNAME=`/bin/hostname`;
	  chomp $HOSTNAME;
	  $OSVERSION=`uname -v`;
	  chomp $OSVERSION;
	  $OSRELEASE=`uname -r`;
	  chomp $OSRELEASE;
	  if ( ($OSVERSION eq "5" && $OSRELEASE ne "2"))
	    {
	       print "\nUnsupported operating system $UNAME$OSVERSION$OSRELEASE!\n";
	       exit;
	    }
       }
     elsif ($UNAME eq "HP-UX")
       {
	  $HOSTNAME=`/bin/hostname`;
	  chomp $HOSTNAME;
	  $OSVERSION=`uname -r`;
	  chomp $OSVERSION;
	  $OSVERSION=~s/\w+\.(\d+)\..*/$1/;
	  $OSRELEASE="";
	  if ($OSVERSION ne "11")
	    {
	       print "\nUnsupported operating system $UNAME$OSVERSION$OSRELEASE!\n";
	       exit;
	    }
       }
     elsif ($UNAME eq "Linux" )
       {
	   my $versionStr = `grep VERSION /etc/SuSE-release`;
	   $UNAME="SuSE-Linux";
	   $HOSTNAME=`/bin/hostname`;
	   chomp $HOSTNAME;
	   $OSVERSION=`grep VERSION /etc/SuSE-release|cut -d '=' -f2|cut -d ' ' -f2|cut -d '.' -f1`;
	   chomp $OSVERSION;
	   if ( $versionStr =~ /\./ )
	   {
	       $OSRELEASE=`grep VERSION /etc/SuSE-release|cut -d '=' -f2|cut -d ' ' -f2|cut -d '.' -f2`;
	       chomp $OSRELEASE;
	   }
	   print "Found $UNAME version: $OSVERSION.$OSRELEASE\n";
       }
       elsif ($UNAME eq "CYGWIN")
       {
	  $UNAME="Windows";
	  $HOSTNAME=`hostname`;
	  chomp $HOSTNAME;
	  $OSVERSION="2000";
	  $OSRELEASE="";
	  $devNull="null";
       }
     
     else
       {
	  $HOSTNAME=`/usr/ucb/hostname`;
	  chomp $HOSTNAME;
	  $OSVERSION="undefined";
	  $OSRELEASE="";
       }
	 
	  @LIBS=qw (libidabase.a libidatdf.a libidaweb.a);
     @MODS=qw (IdaTdfProcess IdaWebProcess);
	  @UNITS=qw (Base WebProcess TdfProcess);
	  		 
  }

sub setLogFileName
  {
     
     ################ set logfilename and PLATFORMDIR #######################
     #
     
     $logfile="logfile.make.$HOSTNAME";
     $PLATFORMDIR="$UNAME$OSVERSION$OSRELEASE";
  }

sub readBuildInfo
  {
    if (( $buildInfoFile eq "BuildInfo.xml" ) &&  ( -r "$PLATFORMDIR/$buildInfoFile" ))
      {
        print "copying $PLATFORMDIR/BuildInfo.xml to BuildInfo.xml\n";
        system ( "cp -f $PLATFORMDIR/BuildInfo.xml BuildInfo.xml");
      }
    elsif ( ( $buildInfoFile ne "BuildInfo.xml" ) && (-r $buildInfoFile) )
      {
        print "found build information $buildInfoFile\n";
      }
    else
      {
        print "BuildInfo.xml missing, please enter build id: ";
        $response=<STDIN>;
        $projectId = "IDA-C-CORE.02.20";
        $ENV{'DM_ROOT'}="/opt/serena/dimensions/10.1/cm";
        print "executing: $scriptpool/dimensions/getBuildInfoForBI.$scriptext $projectId $response\n";
        system ("$scriptpool/dimensions/getBuildInfoForBI.$scriptext $dbName $projectId $response" );
        print ( "copying BuildInfo.xml to $PLATFORMDIR/BuildInfo.xml\n" );
        system ( "cp -f BuildInfo.xml $PLATFORMDIR/BuildInfo.xml");
        if ( ! -r $buildInfoFile )
          {
            die "Can't find BuildInfo.xml file";
          }
      }

    unless ( open (INPUT, "< $buildInfoFile") )
      {
        die "Can't read '$buildInfoFile'!";
      }

    my $xmlBuildInfo;
    read ( INPUT, $xmlBuildInfo, -s INPUT);

    $xmlBuildInfo =~ s/[\n\r]*//g; # get rid of eol characters

    my $buildInfo = XMLin($xmlBuildInfo, ForceArray => [ 'Library' ], ForceArray => [ 'ExternalLibrary' ], 'KeyAttr' => [ ]);

    my $buildId =  $buildInfo->{Software}->{'bi:buildId'};
    my $buildNo;
    my $buildOs;
    ($buildNo,$buildOs) = split ( /\./, $buildId );
    print "Current OS: $PLATFORMDIR and buildOs Number from buildId $buildId is $buildOs\n";
    if ( ($PLATFORMDIR eq "Windows2000") && ($buildOs ne "1" ))
      {
        die "BuildInfo not for current operating system";
      }
    if ( ($PLATFORMDIR eq "AIX52") && ($buildOs ne "2" ))
      {
        die "BuildInfo not for current operating system";
      }
    if ( ($PLATFORMDIR eq "HP-UX11") && ($buildOs ne "4" ))
      {
        die "BuildInfo not for current operating system";
      }
    if ( ($PLATFORMDIR eq "SuSE-Linux10") && ($buildOs ne "7" ))
      {
        die "BuildInfo not for current operating system";
      }
    print "Building IDA for buildId: $buildId\n";

    $copyRight = $buildInfo->{CopyRight} . "\n";
    
    my $libs = $buildInfo->{SoftwareDependencies}->{SourceEnvironment}->{Libraries}->{Library};
    my @libraries = @{$libs};

    foreach $lib (@libraries)
      {
        if ( $lib->{'bi:softwareName'} eq "classlib 03.00" )
          {
            $ENV{'CLHOME'}="$pool/classlib/03.00/" . $lib->{'bi:buildId'};
          }
        if ( $lib->{'bi:softwareName'} =~ /pcpstl 01.*/ )
          {
            my $project;
            my $version;
            ($project,$version) = split (" ", $lib->{'bi:softwareName'});
            $ENV{'STLPATH'}="$pool/stl/$version/" . $lib->{'bi:buildId'};
          }
        if ( $lib->{'bi:softwareName'} =~ /osaapi 02.*/ )
          {
            my $project;
            my $version;
            ($project,$version) = split (" ", $lib->{'bi:softwareName'});
            $ENV{'OSAAPIHOME'}="$pool/osaapi/$version/" . $lib->{'bi:buildId'};
          }
            
      }

    my $extlibs = $buildInfo->{SoftwareDependencies}->{SourceEnvironment}->{ExternalLibraries}->{ExternalLibrary};
    my @extLibraries = @{$extlibs};

    foreach $extlib (@extLibraries)
      {
        if ( ( $extlib->{'bi:name'} =~ /xerces/ ) && ($UNAME ne "Windows"))
          {
            $ENV{'XERCESHOME'}="$pool/".$extlib->{'bi:name'}."/" . $extlib->{'bi:version'};
          }
      }
  }

sub getArguments
  {
     $compile="no";
     $check="no";
     $clean="no";
     $purify="no";
     $optimize="no";
     $debug="no";
     $profiling="no";
     $ignore="no";
     $kill="no";
     $cleanBeforeBuild = "no";
     $cleanBeforeBuildSet = "no";
     $auto = "no";
     $buildInfoFile = "BuildInfo.xml";
     
     if ($ARGV[0] eq "")
       {
	  print "\nNo arguments given - default arguments are compile debug and bi=./BuildInfo.xml\n";
	  print "'build help' gives a list of all possible arguments\n\n";
	  $debug="yes";
	  $compile="yes";
	  return 1;
       }
     
     foreach (@ARGV)
       {
	  if ($_ eq "help")
	    {
	       showHelp(); 
	       exit;
	   }
	  elsif ($_ eq "ignore")
	    {
	       $ignore="yes";
	    }
	  elsif ($_ eq "clean")
	    {
	       $clean="yes";
	    }
	  elsif ($_ eq "compile")
	    {
	       $compile="yes";
	    }
	  elsif ($_ eq "check")
	    {
	       $check="yes";
	       print "Since 'check' is chosen, all other options are ignored!\n";
	       return;
	    }
	  elsif ($_ eq "optimize")
	    {
	       $optimize="yes";
	    }
	  elsif ($_ eq "purify")
	    {
	       $purify="yes";
	    }
	  elsif ($_ eq "debug")
	    {
	       $debug="yes";
	    }
	  elsif ($_ eq "profiling")
	    {
	       $profiling="yes";
	    }
	  elsif ($_ eq "killall")
	    {
	       $kill="yes";
	       print "Since 'killall' is chosen, all other options are ignored!\n";
	       return;
		  }
	  # Profiling has been removed from all. Has to be specified seperately (like build all profiling)
	  elsif ($_ eq "all")
	    {
	       $optimize="yes";
	       $debug="yes";	       
        }
     elsif ($_ eq "cleanbuild")
       {
         $cleanBeforeBuild = "yes";
         $cleanBeforeBuildSet = "yes";
       }
     elsif ($_ eq "nocleanbuild")
       {
         $cleanBeforeBuild = "no";
         $cleanBeforeBuildSet = "yes";
       }
     elsif ($_ eq "auto")
       {
         $cleanBeforeBuild = "yes";
         $auto = "yes";
       }
     elsif ($_ =~ /^bi=/ )
       {
         my $trash="";
         ($trash,$buildInfoFile) = split ( "=" ); 
       }
     elsif ($_ eq "ignorebi")
       {
         $buildInfoFile = "ignore";
       }
	  else
	    {
	       print "$_: unknown argument\n";
	       exit;
	    }
       }
     
     
     if ($optimize eq "no" && $debug eq "no" && $profiling eq "no")
       {
	  print "Using debug as default argument!\n";
	  $debug="yes";
       }
     if ($clean eq "no" && $check eq "no" && $compile eq "no")
       {
	  print "Using compile as default action!\n";
	  $compile="yes";
       }      
  }


sub showHelp 
  {
     print "syntax: build [help] | [[|clean|check|compile|purify|optimize|debug|profiling|all|ignore|killall]]*\n";
     print "\n";
     print "help:          this message\n";
     print "bi=<bif>       give complete path to build info file (bif)\n";
     print "clean:         clean all compiled stuff\n";
     print "compile:       compile\n";
     print "check:         check compilation by analysing $logfile and check the existence of the libraries\n";
     print "ignore:        continue compilation although an error had occured\n";
     print "               and abort compilation\n";
     print "optimize:      compile all units with optimizer\n";
     print "debug:         compile all units with debug information\n";
     print "profiling:     compile all units with profiling information\n";
     print "purify:        instruments all compiled units with purify\n";
     print "all:           is optimize debug (NOT profiling)\n";
     print "killall:       deletes all platform directories\n";
     print "cleanbuild     cleans before building\n";
     print "nocleanbuild   does not clean before building\n";
     print "auto           no interactive questions, cleans before building and exits if ZMLPATH is not set\n";
     print "\n";
     print "Note:          if clean, compile, check is undefined, default action is compile\n";
     print "Note:          if optimize, debug, profiling is undefined, default variant is debug\n";
     print "\n";
     print "Examples:\n";
     print "               build clean - removes all *.o files, tmpdirs, libraries and executables\n";
     print "               build debug ignore - builds the debug libraries and continues if an error occured\n";
     print "               build clean debug - removes all compiled stuff and builds the debug libraries \n";
     print "               build check optimize debug - checks compilation and the libraries in /lib and /libdeb\n";
     print "\n";
  }


sub checkAll
  {
     my $return=0;
     
     $return=checkLogFile();

     if ($return != 0 )
       {
	  print "***** Compilation seems to be not successful!\n\n";
       }
     else
       {
	  print "\n***** Compilation seems to be ok!\n\n";
       }
     
     checkLibAndExe();
  }



sub checkUnit 
  {
     my $unit=shift;
     my $return=0;

     if ($ignore eq "no")
       {
	  $return=checkLogFile();

	  if ($return != 0) 
	    { 
	       print "\nCompilation error in unit $unit - abort compilation!\n";
	       exit;
	    }
       }   
  }



sub checkLogFile 
  {

     my $linkErrors=0;
     my $compilePattern="";
     my $linkPattern1="";
     my $linkPattern2="";
     
     if (!(-f "$logfile"))
       {
	  print "$logfile not found in the current directory!\n";
	  return 1;
       }

     if ($UNAME eq "AIX")
       {
	  if ( $OSVERSION eq "5")
	    {
	       $compilePattern='\(S\)';
	       $linkPattern1='ld\:\s*[0-9\-]+\s*ERROR\s*\:';
	       $linkPattern2='ld\:\s*[0-9\-]+\s*Cannot\s*find';
	    }
	  else
	    {
	       print "Non-supported OS version $OSVERSION!\n";
	       return 1;
	    }
       }
     elsif ($UNAME eq "HP-UX")
       {
	  if ($OSVERSION eq "11")
	    {
	       $compilePattern='Error \d+\:';
	       $linkPattern1='\:\s*Unsatisfied\s*symbols\:';
	       $linkPattern2='ld\:\s*Can\'t\s*find\*library';
	    }	  
	  else
	    {
	       print "Non-supported OS version $OSVERSION!\n"; 
	       return 1;
	    }
       }
     elsif ($UNAME eq "SuSE-Linux")
       {
	  $compilePattern=':\d+\: error';
	  $linkPattern1='\:\s*undefined\*reference\s*to\s*\`';
	  $linkPattern2='ld\:\s*cannot find';
       }
     elsif ($UNAME eq "Windows")
       {
	  $compilePattern='\w\:[^(]*\(\d+\)\s*\:\s*(fatal)*\s*error';
	  $linkPattern1='error\s+LNK\d+\s*\:\s*'; 
	  $linkPattern2='error\s*LNK\d+\:\s*cannot open file';
       }
     else
       {
	  print "Not supported operating system $UNAME!\n";
	  return 1;
       }
    
     if (!open(LOGFILE,"<$logfile"))
       {
	  print "Couldn't open log file $logfile to check the compilation!\n";
       }

     while (<LOGFILE>)
       {
	  if ($_=~ /$compilePattern/)
	    {
	       $err++;
	       print "$_";
	    }

	  # On HP machines print all lines after the link error message
	  if ($linkErrors==1)
	    {
	       $err++;
	       print "$_";
	    }
	  
	  if (($_=~ /$linkPattern1/ || $_=~ /$linkPattern2/) && $_!~ /Undefined\s+symbol\:\s*\.main\s*$/)
	    {
	       $err++;
	       print "$_";
	       if ($UNAME eq "HP-UX")
		 {
		    $linkErrors=1;
		 }
	    }
       }
     
     close (LOGFILE);
     
     if ( $err != 0 )
       {
	  return 1;
       }
     else
       {
	  return 0;
       }
  }



sub checkLibAndExe
  {
     #
     # check existence and size of all libraries
     #

     $err=0;
     
     if ($optimize eq "yes")
       {
	  print "Checking libraries in $PLATFORMDIR/lib...\n";
	  @tmpList=();
	  
	  foreach (@LIBS)
	    {
	       if ($UNAME eq "Windows")
		 {
		    s/(.*)\.a$/$1/;
		    checkExistenceAndSize("$PLATFORMDIR/lib/$_.lib");
		    push @tmpList,"$_.lib";
		 }
	       else
		 {
		    checkExistenceAndSize("$PLATFORMDIR/lib/$_");
		    push @tmpList,"$_";
		 }
	    }
	  
	  checkExtraFiles([@tmpList],"$PLATFORMDIR/lib");	    
       }
     
     if ($debug eq "yes")
       {
	  print "Checking libraries in $PLATFORMDIR/libdeb...\n";
	  @tmpList=();
	    	  
	  foreach (@LIBS)
	    {
	       if ($UNAME eq "Windows")
		 {
		    s/(.*)\.a$/$1/;
		    checkExistenceAndSize("$PLATFORMDIR/libdeb/$_.lib");
		    push @tmpList,"$_.lib";
		    push @tmpList,"$_.pdb";
		    push @tmpList,"$_.idb";
		 }
	       else
		 {
		    checkExistenceAndSize("$PLATFORMDIR/libdeb/$_");
		    push @tmpList,"$_";
		 }
	    }
	  
	  checkExtraFiles([@tmpList],"$PLATFORMDIR/libdeb");	  
       }
     
     
     if ($profiling eq "yes")
       {
	  print "Checking libraries in $PLATFORMDIR/libprof...\n";
	  @tmpList=();
	  	  
	  foreach (@LIBS)
	    {
	       if ($UNAME eq "Windows")
		 {
		    s/(.*)\.a$/$1/;
		    checkExistenceAndSize("$PLATFORMDIR/libprof/$_.lib");
		    push @tmpList,"$_.lib";
		 }
	       else
		 {
		    checkExistenceAndSize("$PLATFORMDIR/libprof/$_");
		    push @tmpList,"$_";
		 }
	    }
	  
	  checkExtraFiles([@tmpList],"$PLATFORMDIR/libprof");
	  
       }


     #
     # Check the modules
     #

     if ($optimize eq "yes")
       {
	  print "Checking modules in $PLATFORMDIR/modules...\n";
	  @tmpList=();
	  
	  foreach (@MODS)
	    {
	       if ($UNAME eq "Windows")
		 {
		    checkExistenceXBitAndSize("$PLATFORMDIR/modules/$_.exe");
		    push @tmpList,"$_.exe";
		 }
	       else
		 {
		    checkExistenceXBitAndSize("$PLATFORMDIR/modules/$_");
		    push @tmpList,"$_";
		 }
	    }
	  
	  checkExtraFiles([@tmpList],"$PLATFORMDIR/modules");	    
       }
     
     if ($debug eq "yes")
       {
	  print "Checking modules in $PLATFORMDIR/modulesdeb...\n";
	  @tmpList=();
	    	  
	  foreach (@MODS)
	    {
	       if ($UNAME eq "Windows")
		 {
		    checkExistenceXBitAndSize("$PLATFORMDIR/modulesdeb/$_.exe");
		    push @tmpList,"$_.exe";
		 }
	       else
		 {
		    checkExistenceXBitAndSize("$PLATFORMDIR/modulesdeb/$_");
		    push @tmpList,"$_";
		 }
	    }
	  
	  checkExtraFiles([@tmpList],"$PLATFORMDIR/modulesdeb");	  
       }
     
  

     
     if ($err==0)
       {
	  print "\n***** All libraries exist and have a size greater than 0!\n";
	  print "***** All processes exist and are executable!\n\n";
       }		 
  }



sub checkExistence 
  {
     
     my $file=shift;     
	
     if (!(-e "$file")) 
       {
	  print "$file does not exist!\n";
	  $err++;
	  return 1;
       }
     
     return 0;
  }



sub checkExistenceAndSize
  {
     my $file=shift;
     
     $return=checkExistence("$file");

     if ($return != 0) 
       {
	  return 1;
       }
     
     if (!(-s "$file")) 
       {
	  print "$file is empty!";
	  $err++;
       }
  }

sub checkExistenceXBitAndSize
  {
     my $file=shift;
     
     $return=checkExistence("$file");

     if ($return != 0) 
       {
	  return 1;
       }
     
     if (!(-s "$file")) 
       {
	  print "$file is empty!";
	  $err++;
	  return;
       }

     if (!(-x "$file")) 
       {
	  print "$file is empty!";
	  $err++;
	  return;
       }
     
  }

sub checkExtraFiles 
  {
     my $fileList=shift;
     my @fileList=@{$fileList};
     my $subdirectory=shift;
     my @dirs=();
     my $dir="";
     my @unexpectedList=();
     my $check="";
     
     if ($err)  # this check requires that all files, which should exist are indeed present.
       {
	  return; 
       }

     $check=opendir(DIR,"$subdirectory");
     
     if (!$check)
       {
	  print "Couldn't open directory $subdirectory!\n";
	  return;
       }
     
     @dirs=sort(readdir(DIR));
     closedir(DIR);
     
          
     
     # Look for unknown files
     foreach $dir (@dirs)
       {
	  $found=0;
	  
	  # Skip .pdb and .idb and .res and .ilk files 
	  if ($dir!~/\.[pi]db$/ && $dir!~/^\.+$/ && $dir!~/\.res$/ && $dir!~/\.ilk$/ && $dir!~/^pcpwnpactivex./)
	    {
	       foreach $file (@fileList)
		 {
		    if ($file eq $dir)
		      {
			 $found=1;
			 last;
		      }
		 }
	       
	       if (!$found)
		 {
		    push @unexpectedList,$dir;
		 }
	    }	 
       }
     
          
     #
     # check return value of previous command, i.e. diff
     #
     
     if ($#unexpectedList >= 0) 
       {
	  print "The following files are not expected to be found directory $subdirectory:\n\n";
	  foreach (@unexpectedList)
	    {
	       print "$_\n";
	    }	 
	  $err++;
       }
     
  }


sub prepareLogFile
  {
     if ($compile eq "yes" || $clean eq "yes")
       {    
	  open(LOGFILE,">$logfile");
	  close LOGFILE;
       }
  }


sub createStructure
  {
	 my $savePath="";
	 
	 #
	 ################ create directories and link include directory #######################
	 #
	 
	 foreach (@SUPPORTED_PLATFORMS)
		{
		  mkdir("$_",0755);
		  $savePath=pwd();
		  chomp ($savePath);
		  
		  chdir("$_");
		  
		  mkdir("bin",0755);
		  mkdir("bindeb",0755);
		  mkdir("modules",0755);
		  mkdir("modulesdeb",0755);
		  mkdir("util",0755);
		  mkdir("utildeb",0755);
		  mkdir("lib",0755);
		  mkdir("libdeb",0755);
		  mkdir("libprof",0755);
		  
		  if ($UNAME ne "Windows" && $_!~ /Windows/)
			 {
				symlink("../dat","dat");
			 }
		  # Unfortunately the classlib doesn't like the link and a supervisor start fails,
		  # if the CL_ROOT_PATH is set to ZMLROOT/PLATFORMDIR- which is necessary for some testcases
		  # Therefore the dat directory is copied not linked :-(
		  elsif ($UNAME eq "Windows" && $_=~ /Windows/ && !(-e "dat"))
			 {
				system("cp -rf ../dat .");
			 }
		  
		  chdir("$savePath");

		}     
  }


sub linkScripts
  {
	 my $source=shift;
	 my $target=shift;
	 my @dirs=();
	 my $dir="";
	 my $check="";
	 
	 $check=opendir(DIR,"$source");
	 
	 if (!$check)
		{
		  print "Couldn't open directory $subdirectory!\n";
		  return;
		}
	 
	 @dirs=sort(readdir(DIR));
	 closedir(DIR);
	 
	 
	 foreach $dir (@dirs)
		{
		  if  (!(-d "$source/$dir"))
			 {
				system("chmod +x $source/$dir");
				
				if ($UNAME ne "Windows" && $target!~ /Windows/ && $dir !~ /\.ksh/)
				  {
					 symlink("../../$source/$dir","$target/$dir");
				  }
				elsif ($UNAME eq "Windows" && $target=~ /Windows/ && !(-e "$target/$dir.lnk") && $dir=~ /\.ksh/)
				  {
					 $dir=~ /(.*)\.win/;
					 $winDir=$1;
					 
					 system("ln -s ../../$source/$dir $target/$winDir");
				  }
			 }
		}     
  }



sub doExecuteAll
  {
     
     my $argument=shift;
     my $savePath="";
     my $unit="";
     my $askPath=0;
     my $response="";
     
     # make IdaStringToEnum.h / .cc from OSA API
     print "generate code for TdfProcess/IdaStringToEnum.h / .cc\n";
     my $saveDir=pwd();
     chdir("TdfProcess");

     system ("chmod u+w IdaStringToEnum.*");
     system ("perl gencode.pl");
     chdir ($saveDir);

     if ($UNAME eq "SuSE-Linux" || $UNAME eq "Windows")
       {
	  $MAKE="make";
       }
     else
       {
	  $MAKE="gmake";
       }
     
     #
     # check imported ZML stuff
     #
     if ($ENV{"CLHOME"} eq "" && $argument eq "")
       {
         print "Warning: Environment variable to classlib libraries CLHOME not set!\n";
         if ( $auto eq "yes" )
           {
             exit;
           }
         $askPath=1;
       }

     
     if (!$askPath && $ENV{"TMP"} eq "" && $argument eq "")
       {
	  print "Warning: Environment variable TMP not set!\n"
       }
    
     #
     # Note: changing into the directory is neccessary for checking relative pathes
     #

     $saveDir=pwd();


     if (!(-d "$ENV{'CLHOME'}") && $argument eq "")
       {
	  print "The CLHOME $ENV{'CLHOME'} does not exist!\n";
	  $askPath=1;
       }


     chdir("modulessrc");
     chdir("$saveDir");

     if ($ENV{'TMP'} && !(-d "$ENV{'TMP'}") && $argument eq "")
       {
	  print "TMP contains an invalid path $ENV{'TMP'}!\n";
       }

     #
     # Ask user, if the settings are correct
     #
     
     if (!$askPath && $argument eq "")
       {
         if ( $buildInfoFile eq "ignore" )
           {
             print "\nYour environment delivers the following settings:\n\n";
           }
         else
           {
             print "\nYour BuildInfo.xml delivers the following settings:\n\n";
           }
         print "CLHOME=       $ENV{'CLHOME'}\n";
         print "OSAAPIHOME=   $ENV{'OSAAPIHOME'}\n";
         print "TMP=          $ENV{'TMP'}\n";
         print "STLPATH=      $ENV{'STLPATH'}\n";
         print "XERCESHOME=   $ENV{'XERCESHOME'}\n\n";

     if ( $auto eq "no" )
       {
         print "Are you satisfied with these settings (Type 'n' or 'N' for NO): ";

         $response=<STDIN>;
         chomp($response);
         
         if (lc($response) eq "n" || lc($response) eq "no")
           {
             $askPath=1;
           }
       }
   }

     #
     # Ask user for new CLHOME
     #

     if ($askPath && $auto eq "no" )
       {

	  # Ask for CLHOME
	  print "\nPlease enter the new values (Enter will keep the current value)\n\n";
	  print "CLHOME [$ENV{'CLHOME'}]: ";

	  $response=<STDIN>;
	  chomp($response);

	  chdir("binsrc");
	  
	  if ($response eq "" && ($ENV{'CLHOME'} eq "" || !(-d "$ENV{'CLHOME'}")))
	    {
	       print "\n\nSince no valid CLHOME was specified, the build scipt is aborted!\n";
	       exit;
	    }
	  elsif ($response ne "" && !(-d "$response"))
	    {
	       print "\n\nSince no valid CLHOME was specified, the build scipt is aborted!\n";
	       exit;
	    }
	  elsif ($response ne "")
	    {
	       $ENV{'CLHOME'}=$response;
	       print "\nWarning: The new setting is only valid for the current build session!\n\n"
	    }

	  
	  # Ask for TMP
	  print "TMP [$ENV{'TMP'}]: ";

	  $response=<STDIN>;
	  chomp($response);
     
	  chdir("$saveDir");
	  chdir("modulessrc");

	  if ($response eq "" && ($ENV{'TMP'} eq "" || !(-d "$ENV{'TMP'}")))
	    {
	       print "\nWarning: TMP contains an invalid path!\n";	       
	    }
	  elsif ($response ne "" && !(-d "$response"))
	    {
	       print "\nWarning: TMP contains an invalid path!\n";
	       exit;
	    }
	  elsif ($response ne "")
	    {
	       $ENV{'TMP'}=$response;
	       print "\nWarning: The new setting is only valid for the current build session!\n\n"
	    }
	  
	   chdir("$saveDir");
   } # end if $askPath

     if ( ($clean eq "no") && ($cleanBeforeBuildSet eq "no") && ($auto eq "no") )
       {
         print "\n" . "Make clean before build y/n ? [n]: ";
         $response=<STDIN>;
         chomp($response);
         if ( $response eq "y" || $response eq "Y" )
           {
             $cleanBeforeBuild = "yes";
           }
       }


     #
     # Prepare log file and start building
     #
     

     open(LOG,">$logfile");
     
     print LOG "############################## CONFIGURATION ###################################\n\n";
     print LOG "CLHOME    = $ENV{'CLHOME'}\n";
     print LOG "TMP        = $ENV{'TMP'}\n\n";
     print LOG "################################################################################\n";

     close (LOG);


     
     #
     # The three seperate loops are not combined to a single one,
     # because the different versions should be compiled one after the other.
     # Reason: If there are source code bugs they should be detected by the compiler as soon as possible
     #

     print "working on $UNAME\n";

     if ($optimize eq "yes")
       {
	       
	       # Make clean must NO longer run on Windows systems before compilation (d36660)
          if ( $cleanBeforeBuild eq "yes" )
            {
              foreach $unit (@UNITS)
                {
                  $savePath=pwd();
                  chdir("$unit");
                  
                  
                  print "\n$MAKE $unit clean \optimize\n";
                  print "==============\n";
                  
                  if (!defined($childpid=fork()))
                    {
                      die "Fork wasn't possible!";
                    }
                  elsif ($childpid==0)
                    {
                      
                      exec("$MAKE -k OPTIMIZE=1 clean >> ../$logfile 2>> ../$logfile");
                    }
		    
                  else {wait}
		
	       
                  chdir("$savePath");
                  checkUnit("$unit");
                }
           
            }
          
          foreach $unit (@UNITS)
            {
              $savePath=pwd();
              chdir("$unit");
              
              
              print "\n$MAKE $unit $argument\optimize\n";
              print "==============\n";
              
              if (!defined($childpid=fork()))
                {
                  die "Fork wasn't possible!";
                }
              elsif ($childpid==0)
                {
		     if ($purify eq "yes")
		     {
			 exec("$MAKE -k OPTIMIZE=1 PURIFY=1 $argument >> ../$logfile 2>> ../$logfile");
		     }
		     else
		     {
			 exec("$MAKE -k OPTIMIZE=1 $argument >> ../$logfile 2>> ../$logfile");
		     }
		 }
              
              else {wait}
              
              
              chdir("$savePath");
              checkUnit("$unit");
            }
        }

     if ($debug eq "yes")
       {
         if ( $cleanBeforeBuild eq "yes" )
           {
             foreach $unit (@UNITS)
               {
                 $savePath=pwd();
                 chdir("$unit");
                     
                 print "\n$MAKE $unit clean debug \n";
                 print "==============\n";
                     
                 if (!defined($childpid=fork()))
                   {
                     die "Fork wasn't possible!";
                   }
                 elsif ($childpid==0)
                   {
                     exec("$MAKE -k DEBUG=1 clean >> ../$logfile 2>> ../$logfile");
                   }
                 
                 else {wait}
                 
                 chdir("$savePath");
                 checkUnit("$unit");
               }
            }
          
         foreach $unit (@UNITS)
           {
             $savePath=pwd();	       
             chdir("$unit");

             print "\n$MAKE $unit $argument\debug\n";
             print "==============\n";
             
             # Make clean must NO longer run on Windows systems before compilation #d36660
             
             if (!defined($childpid=fork()))
               {
                 die "Fork wasn't possible!";
               }
             elsif ($childpid==0)
               {
		   if ($purify eq "yes")
		   {
		       exec ("$MAKE -k DEBUG=1 PURIFY=1 $argument >> ../$logfile 2>> ../$logfile");
		   }
		   else
		   {
		       exec ("$MAKE -k DEBUG=1 $argument >> ../$logfile 2>> ../$logfile");
		   }

               }
             
             else {wait}
		 	       
             chdir("$savePath");
             checkUnit("$unit");
           }
       }

     if ($profiling eq "yes")
       {
	  foreach $unit (@UNITS)
	    {
	       $savePath=pwd();
	       chdir("$unit");

	       print "\n$MAKE $unit $argument\profiling\n";
	       print "==============\n";
	       
	       # Make clean must NO LONGER run on Windows systems before compilation (d36660)
	      
		    if (!defined($childpid=fork()))
		      {
			 die "Fork wasn't possible!";
		      }
		    elsif ($childpid==0)
		      {
			 exec("$MAKE -k PROFILING=1 $argument 1>> ../$logfile 2>> ../$logfile");
		      }
		    
		    else {wait}
		 
	       
	       chdir("$savePath");
	       checkUnit("$unit"); 
	    }
       }
  }


sub pwd()
  {
     my $pwd="";

     $pwd=`pwd`;
     chomp ($pwd);
     
     if ($UNAME eq "Windows")
       {
	  
	  if ($pwd=~/^\/cygdrive\//)
	    {
	       $pwd=~ s/^\/cygdrive\/(\w+)\//$1\:\//;
	    }
       }

     return $pwd;
       
  }


sub killAll()
  {
     $counter=0;
     local $baseDir="";

     $baseDir=pwd();
     chomp($baseDir);

     lookInTree($baseDir);
  }





sub lookInTree
  {
     
     my $actDir=shift;     
     my $subDir=shift;
     my $dir="";
     my @dirs=();
     my $platformDir="";
     my $check=0;


     # Add subDir/Name...
     if ($subDir)
       {
	  $actDir.= "/$subDir";
       }
     

     # Kill the platform directories in the current directory
     foreach $platformDir (@SUPPORTED_PLATFORMS)
       {
	  # Skip the include links in Windows because the include directory would be killed
	  if ($actDir eq $baseDir && -e "$actDir/$platformDir/dat" && $UNAME eq "Windows" && $platformDir!~/win/i)
	    {
	       system("rm -rf $actDir/$platformDir/lib");
	       system("rm -rf $actDir/$platformDir/libdeb");
	       system("rm -rf $actDir/$platformDir/bindeb");
	       system("rm -rf $actDir/$platformDir/modulesdeb");
	       system("rm -rf $actDir/$platformDir/bin");
	       system("rm -rf $actDir/$platformDir/modules");
	       system("rm -rf $actDir/$platformDir/libprof");
	       system("rm -rf $actDir/$platformDir/libprof");
	    }
	  elsif (-e "$actDir/$platformDir")
	    {
	       system("rm -rf $actDir/$platformDir");
	    }
       }
          
     # open and read the directory
     $check=opendir(DIR,"$actDir");
     
     if (!$check)
       {
	  print "Couldn't open directory $actDir\n";
	  return;
       }
     
     @dirs=sort(readdir(DIR));
     closedir(DIR);
     
     
             
      
     # Go through the subdirectories (except . and ..)
   DIR:
     foreach $dir (@dirs)
       {
	  	  
	  # Skip .. and . and ... and ........... (if existing)
	  if ($dir=~ /^\.+$/)
	    {
	       next DIR;
	    }

	    
	  if (-d "$actDir/$dir")
	    {
	       lookInTree($actDir,$dir);	       
	    }	  
       }
  }

	  
	  
	  
	  
	  
