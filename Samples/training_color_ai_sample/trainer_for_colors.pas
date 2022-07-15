function realToStr(r:real;a:integer):string;
begin
var p:=Power(10,a);
Result:=(Round(r*p)/p).ToString();
end;

type Color=record
r,g,b:real;
constructor(r1,g1,b1:real);
begin
r:=r1;
g:=g1;
b:=b1;
end;
function distance(another:color):real;
begin
Result:=(Abs(r-another.r)+Abs(g-another.g)+Abs(b-another.b))/3.0;
end;
function toString(acc:integer):string;
begin
Result:=realToStr(r,acc)+','+realToStr(g,acc)+','+realToStr(b,acc);
end;
end;

function getRandomReal(f,t:real):real;
begin
Result:=f+(t-f)*(Random(1000000)/1000000.0);
end;

function getRandomColor():color;
begin
Result:=new Color(getRandomReal(0,1),getRandomReal(0,1),getRandomReal(0,1));
end;

const
accuracy=5;

begin
Randomize();
Writeln('Dataset size:');
var linesCount:=ReadInteger();
var arr:=new string[linesCount];
var yellow:=new Color(1,1,0);
var violet:=new Color(0.93,0.12,0.93);
for var i:=1 to linesCount do
begin
var c:=getRandomColor();
var d1:=1-c.distance(yellow);
var d2:=1-c.distance(violet);
arr[i-1]:=c.toString(accuracy)+' '+realToStr(d1,accuracy)+','+realToStr(d2,accuracy);
end;
WriteAllLines('train.txt', arr);
Writeln('finished');
end.
