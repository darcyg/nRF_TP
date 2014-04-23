[CmdletBinding()]
Param(
   [switch]$build,
   [string]$address,
	
   [switch]$upload,
   [string]$comport,
   
   [switch]$clean,
   [switch]$vv,
   
   [string][AllowNull()]$garbage1,[string][AllowNull()]$garbage2,[string][AllowNull()]$garbage3,[string][AllowNull()]$garbage4
)

#!!MODIFY THIS!!
$arduinoext="C:\Applications\eclipse\arduino_ext"
$nRFTP_root="C:\Workspace\nRF_TP"
Set-Location -Path $nRFTP_root


#build output fodler inside $nRFTP_root folder
$builddir="Release"

#used programs
$arduino_uploader = $arduinoext+"\ArduinoUploader"
$compiler = $arduinoext+"\bin\avr-g++"
$linker = $arduinoext+"\bin\avr-gcc.exe"
$objcp = $arduinoext+"\bin\avr-objcopy"
$avrsize = $arduinoext+"\bin\avr-size"

############################# Building #############################
if ($build -and -not $clean){
    "=========================================="
    " Building nRF_TP with address "+ $address
    "=========================================="
    ""
        
    #Flags
    $compiler_flags = "-Os -O2 -x c++ -mmcu=atmega328p -DF_CPU=16000000L -D__AVR_ATmega328P__  -fno-exceptions -ffunction-sections -fdata-sections -DARDUINO=105 -DSELF_ADDRESS="+$address
    $linker_flags = "-s -Wl,--gc-sections -lm"
    $objcp1_flags = " -O ihex -R .eeprom -R .eesafe "
    $objcp2_flags = " --no-change-warnings -j .eeprom --change-section-lma .eeprom=0 -O ihex "

    #output files
    $linker_out = $builddir+"\nRF_TP_nano328.elf"
    $main_out = $builddir+"\nRF_TP.hex.hex"

    #compiler include search path
    $includes ="-I. "`
    +"-InRFTP -InRFTP\Message "`
    +"-InRFTP\Util "`
    +"-IRF24 "`
    +"-I"+$arduinoext+"/arduino\hardware\arduino\cores\arduino "`
    +"-I"+$arduinoext+"/arduino\libraries "`
    +"-I"+$arduinoext+"/arduino\hardware\arduino\variants\eightanaloginputs "`
    +"-I"+$arduinoext+"\arduino\libraries\SPI "`

    #collect source files and create their object names
    $sources = @(dir -name -recurse -Include @("*.cpp", "*.ino"))
    $objs = @( dir -name -recurse -Include @("*.cpp", "*.ino") |%{'{0}\{1}.o' -f $builddir,  $_.Substring(0, $($_.IndexOf(".")))} )
               
    #copy directory structure to build dir
    if ($vv){
	  "Creating directory structure..."
	    Robocopy.exe RF24 $($builddir+"\RF24") /e /xf * 
		Robocopy.exe nRFTP $($builddir+"\nRFTP") /e /xf * 
	} else {
	    Robocopy.exe RF24 $($builddir+"\RF24") /e /xf * > $null
		Robocopy.exe nRFTP $($builddir+"\nRFTP") /e /xf * > $null
	}


    #build the arduino core libs
    "-- Building arduino core libs..."
    $build_core = $( $arduino_uploader + " " +$nRFTP_root +"\. nano328 - 8 0 " + $nRFTP_root+"\"+$builddir + "\lib_nano328_8.a " + $nRFTP_root+"\"+$builddir + "\core_nano328_8.a")
    if ($vv) {
	  $build_core
      iex $build_core
    } else  {
      iex $build_core > $null
    }
    ""

    #build the source files
    "-- Building sources..."
    for($i=0;$i -lt $objs.Length;$i++){
      $cmd = $($compiler+" "+$compiler_flags+" "+$includes+" -c "+$sources[$i]+" -o "+$objs[$i])
      if ($vv){
        $cmd
      } else {
        $sources[$i]
      }
      iex $cmd
    }
    ""

    "-- Linking..."
    # concatenate object file names to linking
    $allobjs = ""
    foreach ($obj in $objs){
      $allobjs = $allobjs + " " +$obj
    }

    #arduino core libs
    $coreobjs = " build\lib_nano328_8.a build\core_nano328_8.a "

    #create the linker command
   # $link_cmd = $($linker+ " -o "+$linker_out + $allobjs + " -s -mmcu=atmega328p " + $coreobjs +$linker_flags  )
    $link_cmd = $($linker+ " -o build\nRF_TP_nano328.elf build\nRF_TP.o build\nRFTP\Message\Header.o build\nRFTP\Message\Message.o build\nRFTP\Message\PingMessage.o build\nRFTP\Message\RouteRequestMessage.o build\nRFTP\nRF24L01_PhysicalLayer.o build\nRFTP\nRFTP.o build\nRFTP\Util\ByteBuffer.o build\nRFTP\Util\Util.o build\RF24\RF24.o   -s -mmcu=atmega328p C:\Workspace\nRF_TP\code\build\/lib_nano328_8.a C:\Workspace\nRF_TP\code\build\/core_nano328_8.a -s -Wl,--gc-sections  -lm")
    if ($vv) {$link_cmd}
    #hack to linking: copy the linker command to a batch file and run it from it, otherwise the -Wl,--gc-sections doesn't work
    $link_cmd | Set-Content linkcmd -Encoding ASCII
    "@echo off & Setlocal EnableDelayedExpansion & for /f `"delims=`" %%x in (linkcmd) do set Linkcmd=%%x & call !Linkcmd!" | Set-Content link.bat -Encoding ASCII
    ./link.bat
    
    #remove temporary files 
    rm link.bat
    rm linkcmd

    #create hex file from elf
    
   
    $objcp1 = $objcp+$objcp1_flags+" "+$linker_out+" "+$main_out
    $objcp1 | Set-Content obj_cp -Encoding ASCII
    "@echo off & Setlocal EnableDelayedExpansion & for /f `"delims=`" %%x in (obj_cp) do set ObjCpCmd=%%x & call !ObjCpCmd!" | Set-Content objcp1.bat -Encoding ASCII
    ./objcp1.bat
    
    #$objcp1 = $objcp+$objcp1_flags+" "+$linker_out+" "+$main_out
    $objcp2 = $objcp+$objcp2_flags+" "+$linker_out+" "+$linker_out+".eep.hex"
    if ($vv) {$objcp1}
   # iex $objcp1
    if ($vv) {$objcp2}
    iex $objcp2
    ""
    
    "-- Building done. "
    ""
    
    # print output size
    $sizecmd = $avrsize + " --mcu=atmega328p --format=avr " + $linker_out
    iex $sizecmd
}

############################# Uploading #############################
if ($upload-and -not $clean){
    cp $($builddir+"\nRF_TP.hex") $($builddir+"\nRF_TP.hex.hex")
    if ($vv) {$arduino_uploader + $nrftp_root+"\"+$builddir+"\nRF_TP.hex 17 "+$comport}
    & $arduino_uploader $($nrftp_root+"\"+$builddir+"\nRF_TP.hex") 7 $comport > $null
    kill $PID
	rm $($builddir+"\nRF_TP.hex.hex")
}

############################# Cleaning ##############################
if ($clean){
    rm $builddir -recurse
}