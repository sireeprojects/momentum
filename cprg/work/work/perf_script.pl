#!/usr/bin/perl

#clear the screen
system ("clear"); 

@txn_cnt       = ( 35000, 80000, 150000);
@txn_size      = (46, 727, 1500, 9216);
@num_inst      = (10, 5, 1);
@repeat        = (on, off);
@tx_pcap       = (on, off);
@rx_pcap       = (on, off);

$test_cmd='';
$log_file1="tmp/irun.log";
$log_file2="tmp/SCEMI20.prof";
$log_file3="tmp/ixcom.log";
$folder_name='';

my $test_mode ='';
my $status = 0;
my @error_check_arr = ("\\*E","\\*F" , "Assertion [0-9]+ failed");


if ( !-d "log")
{
   system ("mkdir log");
}

open(my $fh, '>', 'log/report_cpp_run.txt');
print $fh "*--------------- \n RUN REPORT \n*--------------- \n";
open(my $fh_st, '>', 'log/report_status.txt');
print $fh_st "*--------------- \n Status REPORT \n*--------------- \n";

print $fh " Launching Test: \n ";
print $fh "\n\n--------------------------------------------------------------------------------------------------------------------------------------\n";

foreach $my_num_inst (@num_inst) {
   foreach $my_txn_cnt (@txn_cnt) {
      foreach $my_txn_size (@txn_size) {
         foreach $my_repeat (@repeat) {
            foreach $my_tx_pcap (@tx_pcap){
               foreach $my_rx_pcap (@rx_pcap){
                  if ($my_repeat eq "on" and $my_tx_pcap eq "on")
                  {
#                    NO RUN   
                     next;
                   }
                  if ($my_repeat eq "off" and $my_tx_pcap eq "off" and $my_rx_pcap eq "on")
                  {
#                    NO RUN   
                     next;
                  }

                  if ($my_repeat eq "off" and $my_tx_pcap eq "off" and $my_rx_pcap eq "off" )
                  {
                     $test_mode = " REGULAR MODE";

                  }
                  if ($my_repeat eq "on" and $my_tx_pcap eq "off" and $my_rx_pcap eq "off" )
                  {
                     $test_mode = " REPEAT MODE";

                  }
                  if ($my_repeat eq "off" and $my_tx_pcap eq "on" and $my_rx_pcap eq "off" )
                  {
                     $test_mode = " PCAP TX ONLY MODE ";

                  }
                  if ($my_repeat eq "on" and $my_tx_pcap eq "off" and $my_rx_pcap eq "om" )
                  {
                     $test_mode = " PCAP RX ONLY MODE ";

                  }
                  if ($my_repeat eq "off" and $my_tx_pcap eq "on" and $my_rx_pcap eq "on" )
                  {
                     $test_mode = " PCAP BOTH MODE ";

                  }


                  $folder_name = "inst\_$my_num_inst\_txncnt\_$my_txn_cnt\_txnsize\_$my_txn_size\_repeat\_$my_repeat\_txpcap\_$my_tx_pcap\_rxpcap\_$my_rx_pcap";
                  $folder = "log/$folder_name";
                  print $fh "\n$test_mode \n  ";
                  $test_cmd = "make hwRun".
                     " ETH_NOF_ENVS=$my_num_inst".
                     " TXN_COUNT=$my_txn_cnt".
                     " TXN_SIZE=$my_txn_size".
                     " REPEAT=$my_repeat".
                     " RX_PCAP=$my_rx_pcap".
                     " TX_PCAP=$my_tx_pcap".
                     " BITMODE=64".
                     "\n";
                  print $fh "$test_cmd  Folder : $folder_name\n";
                  if (!-d $folder and !-f "$folder/irun.log" )
                  {
                     print $fh "  Status: Running\n ";
                     system("$test_cmd");
                     system ("mkdir log/$folder_name");
                     system ("cp $log_file1 log/$folder_name/");
                     system ("cp $log_file2 log/$folder_name/");
                     system ("cp $log_file3 log/$folder_name/");
                     system ("rm -rf tmp/hw_lib/ ");
                     system ("rm -rf tmp/irun.log ");
                  } 
                  else            
                  {
                     print $fh "  Status: Test exists\n";
                  }
#                  Check Run Flow errors  
                   open (my $handle, '<',"log/$folder_name/irun.log" );
                   while (my $row = <$handle>) {
                      chomp $row;
                      foreach my $j (@error_check_arr)
                      {
                         if ($row =~ m/$j/) {
                            $status = 1;
                            next;
                         }
                      }                             
                      if ($row =~ ("Total number of frames received *: *$my_txn_cnt") )
                      {
                         $counter = $counter + 1;
                      }
                   }
                   close $handle;
                   if ($status or ($counter ne $my_num_inst)){
                      print $fh_st "Instance: $my_num_inst Txn Count: $my_txn_cnt Txn Size: $my_txn_size $test_mode\n Folder name : $folder_name \n $test_cmd \n\n";                                 }
                           
# Resseting flags
      $counter = 0;
      $status = 0 ;
               }
            }
         }
         print $fh "\n\n--------------------------------------------------------------------------------------------------------------------------------------\n";

      }
   }
   print $fh "\n******* CLEANING ******* \n\n";
   system ("make clean ");
} 
