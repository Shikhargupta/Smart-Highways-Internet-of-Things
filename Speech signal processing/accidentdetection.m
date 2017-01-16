function[a]=accidentdetection(d)

   MFCC_train=[30.3126
    1.5926
    0.2832
   -0.4334
   -0.4859
   -0.2366
    0.2179
    0.1639
    0.0296
   -0.0192
   -0.0067
   -0.0101
   -0.0104]
 MFCC_Test=wavconversion(d);
 E_distance = sqrt(sum((MFCC_train - MFCC_Test) .^ 2));
 if(E_distance<=20)
     display('Accident');
     a=1;
 end
 if(E_distance>20)
     display('High Intensity sound but not an Accident')
     a=0;
 end
 
end