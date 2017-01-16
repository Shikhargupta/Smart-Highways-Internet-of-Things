function[a,l]=a_dtctn(d,l)

   MFCC_train=[        5.4359
    1.2796
   -0.2358
   -0.6747
   -0.1757
   -0.1129
   -0.0897
   -0.0409
   -0.0666
   -0.1176
   -0.0527
   -0.0099
    0.0309]
 MFCC_Test=wavconversion(d);
 E_distance = 100*sqrt(sum((MFCC_train - MFCC_Test) .^ 2))
 if(E_distance<=51)
     display('Accident');
     a=1;
 end
 if(E_distance>51)
     display('High Intensity sound not an Accident')
     a=0;
 end
 
 
end