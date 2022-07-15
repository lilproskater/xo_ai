uses GraphAbc;

type
    cell = class
        val: integer;
        anim: real = 0.01;
        procedure draw(x, y: integer);
        begin
            SetFontSize(Round(anim * 128));
            SetFontColor(val = -1 ? clblue : val = 1 ? clred : clblack);
            DrawTextCentered(x, y, val = -1 ? 'O' : val = 1 ? 'X' : ' ');
        end;
        
        procedure tick();
        begin
            anim := min(anim + 0.05, 1);
        end;
        
        procedure move(v: integer);
        begin
            val := v;
            anim := 0.01;
        end;
    end;

var
    map := new cell[9];
    pulse := new list<integer>();
    allowInput := true;

procedure InitGame();
begin
    allowInput := true;
    pulse.Clear();
    map := new cell[9];
    for var i := 0 to map.Length - 1 do
        map[i] := new cell();
end;

procedure InitWindow();
begin
    SetWindowSize(600, 600);
    Window.IsFixedSize := true;
    SetWindowCaption('XO AI - Game');
    CenterWindow();
    LockDrawing();
end;

procedure ProcessPulse();
begin
    var time := Milliseconds();
    for var i := 0 to pulse.Count - 1 do
    begin
        map[pulse[i]].anim := ((Sin(DegToRad((time + i * 300) / 10.0)) + 2.0) / 3.0);
    end;
end;

procedure DrawBorders();
begin
    SetPenWidth(5);
    Line(200, 0, 200, 600);
    Line(400, 0, 400, 600);
    Line(0, 200, 600, 200);
    Line(0, 400, 600, 400);
end;

procedure DrawMap();
begin
    SetFontSize(128);
    for var i := 0 to map.Length - 1 do
    begin
        var x := i mod 3 * 200 + 100;
        var y := i div 3 * 200 + 100;
        map[i].draw(x, y);
        map[i].tick();
    end;
end;

function GetAiMove(): integer;
begin
    var process := new System.Diagnostics.Process();
    process.StartInfo.FileName := 'cmd.exe';
    process.StartInfo.WorkingDirectory := System.Environment.CurrentDirectory;
    var input := '';
    foreach var c in map do
        input += c.val + ' ';
    process.StartInfo.Arguments := '/c echo ' + input + ' | xo_ai.exe';
    process.StartInfo.CreateNoWindow := true;
    process.StartInfo.RedirectStandardInput := true;
    process.StartInfo.RedirectStandardOutput := true;
    process.StartInfo.UseShellExecute := false;
    process.Start();
    result := StrToInt(process.StandardOutput.ReadLine());
end;

function checkWin(v: integer): boolean;
label skip;
begin
    var wins: array of array of integer = ((0, 1, 2), (3, 4, 5), (6, 7, 8), (0, 3, 6), (1, 4, 7), (2, 5, 8), (0, 4, 8), (6, 4, 2));
    for var i := 0 to wins.Length - 1 do
    begin
        for var j := 0 to wins[i].Length - 1 do
            if map[wins[i][j]].val <> v then
                goto skip;
        pulse.AddRange(wins[i]);
        Result := true;
        exit;
        skip: 
    end;
end;

procedure mouseClick(x, y, b: integer);
begin
    if not allowInput then
    begin
        InitGame();
        exit;
    end;
    var cellId := (x div 200) + (y div 200) * 3;
    if map[cellId].val <> 0 then
        exit;
    map[cellId].move(1);
    if checkWin(1) then
    begin
        allowInput := false;
        exit;
    end;
    sleep(200);
    var aiMove := GetAiMove();
    if aiMove = -1 then
    begin
        var ar: array of integer = (0, 1, 3, 2, 4, 6, 5, 7, 8);
        pulse.AddRange(ar);
        allowInput := false;
        exit;
    end;
    map[aiMove].move(-1);
    if checkWin(-1) then
        allowInput := false;
end;

begin
    InitWindow();
    InitGame();
    OnMouseDown += mouseClick;
    while true do
    begin
        ClearWindow();
        DrawBorders();
        ProcessPulse();
        DrawMap();
        Redraw();
        Sleep(20);
    end;
end.
