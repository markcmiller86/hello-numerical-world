#!/bin/sh

tmpfil=/tmp/$1_$$_$USER.curve
lenx=$(tail -1 $1/$1_soln_final.curve | tr -s ' ' | cut -d' ' -f2)

# Compute midpoint of wall
lenx2=$(dc << EOF
4
k
$lenx
2.0
/
p
EOF)

# Compute left end of pipe
p0=$(dc << EOF
4
k
$lenx2
0.05
-
p
EOF)

# Compute right end of pipe
p1=$(dc << EOF
4
k
$lenx2
0.05
+
p
EOF)

# Pour all the data into a single file
cat $1/$1_soln_final.curve > $tmpfil
echo "# Water_Freeze" >> $tmpfil
echo "0 273.15" >> $tmpfil
echo "$lenx 273.15" >> $tmpfil
echo "# Pipe" >> $tmpfil
echo "$p0 273.15" >> $tmpfil
echo "$p1 273.15" >> $tmpfil

visit -cli -launchengine << EOF 1>/dev/null 2>&1
import sys, time
OpenDatabase("$tmpfil")
A = AnnotationAttributes()
A.userInfoFlag=0
A.databaseInfoFlag=0
A.legendInfoFlag=0
A.axes2D.xAxis.title.userUnits = 1
A.axes2D.xAxis.title.userTitle = 1
A.axes2D.xAxis.title.units = "meters"
A.axes2D.xAxis.title.title = "Distance"
A.axes2D.yAxis.title.userUnits = 1
A.axes2D.yAxis.title.userTitle = 1
A.axes2D.yAxis.title.units = "Kelvin"
A.axes2D.yAxis.title.title = "Temperature"
SetAnnotationAttributes(A)
AddPlot("Curve","Temperature")
AddPlot("Curve","Water_Freeze")
ca = CurveAttributes()
ca.lineStyle = ca.DASH
ca.symbol = ca.Plus
SetPlotOptions(ca)
AddPlot("Curve","Pipe")
ca = CurveAttributes()
ca.showPoints = 1
SetPlotOptions(ca)
DrawPlots()
vc = GetViewCurve()
vc.domainCoords = (0, $lenx)
SetViewCurve(vc)
print "Ctrl-C to exit or auto-exit after 120 seconds"
time.sleep(120)
sys.exit(0)
EOF
