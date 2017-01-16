
delete(instrfindall);
s = serial('COM6');

set(s,'BaudRate',9600,'InputBufferSize',10000);
fopen(s);
data = zeros(1,5000);
count = 0; miss = 0;
while (1)
    %flushinput(s);
    while(s.BytesAvailable)
       out = fscanf(s);
       miss = 0;
       disp(out);
       jk = strsplit(out,'s');
       for i=1:size(jk,2)
           judge = isempty(char(jk(i)));
           if ~judge
               count = count + 1;
               data(count+miss) = str2double(jk(i));
           else miss = miss - 1;
           end

       end
       if ((count > 0) && (s.BytesAvailable==0))
           acci = a_dtctn(data(1:count));
           fprintf('total number of received data:%d\n',count);
           fprintf('Accident happended:%d\n',acci);
           data(1:count) = 0;
           count = 0;
           fprintf(s,'%d',acci);
       end
    end
end