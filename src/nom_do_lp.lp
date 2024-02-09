\ENCODING=ISO-8859-1
\Problem name: IloCplex

Minimize
 obj_10p: - 2 C(1)#20 - 2 C(1)#21
Subject To
 Source_1#0:                 c(1,1)#8 + c(1,2)#10 + c(1,3)#12 + c(1,4)#14
                             - RgSource_1#0  = 0
 Source2_1#1:                f(1,1,2)#4 - c(1,1)#8 - c(1,2)#10 - c(1,3)#12
                             - c(1,4)#14 - RgSource2_1#1  = 0
 Source3_1#2:                f(1,2,1)#5  = 0
 Source_1#3:                 c(1,1)#9 + c(1,2)#11 + c(1,3)#13 + c(1,4)#15
                             - RgSource_1#3  = 0
 Source2_1#4:                f(1,1,2)#6 - c(1,1)#9 - c(1,2)#11 - c(1,3)#13
                             - c(1,4)#15 - RgSource2_1#4  = 0
 Source3_1#5:                f(1,2,1)#7  = 0
 Target_1#6:                 f(1,2,3)#0 + f(1,2,1)#5  = 0
 Target2_1#7:                f(1,3,2)#1 + f(1,1,2)#4 - RgTarget2_1#7  = 0
 Target_1#8:                 f(1,3,2)#3  = 0
 Target2_1#9:                f(1,2,3)#2 - RgTarget2_1#9  = 0
 Degree_1_3#10:              f(1,3,2)#1 - RgDegree_1_3#10  = 0
 Degree_1_2#11:              f(1,2,3)#2 + f(1,2,1)#7 - RgDegree_1_2#11  = 0
 Flow_3_1#12:                - f(1,2,3)#0 + f(1,3,2)#1  = 0
 Flow_2_1#13:                f(1,2,3)#2 - f(1,3,2)#3 - f(1,1,2)#6 + f(1,2,1)#7
                              = 0
 Length_1#14:                100 f(1,2,3)#0 + 100 f(1,3,2)#1 + 100 f(1,1,2)#4
                             + 100 f(1,2,1)#5 - RgLength_1#14  = 0
 Length_1#15:                100 f(1,2,3)#2 + 100 f(1,3,2)#3 + 100 f(1,1,2)#6
                             + 100 f(1,2,1)#7 - RgLength_1#15  = 0
 MaxUsedSlicePerLink_2_1#16: 4 f(1,2,3)#0 + 4 f(1,3,2)#1 + c(1,1)#8
                             + 2 c(1,2)#10 + 3 c(1,3)#12 + 4 c(1,4)#14
                             - maxSlice(1)#17 - RgMaxUsedSlicePerLink_2_1#16
                              = -4
 MaxUsedSlicePerLink_1_1#17: 4 f(1,1,2)#4 + 4 f(1,2,1)#5 + c(1,1)#8
                             + 2 c(1,2)#10 + 3 c(1,3)#12 + 4 c(1,4)#14
                             - maxSlice(0)#18 - RgMaxUsedSlicePerLink_1_1#17
                              = -4
 MaxUsedSlicePerLink_2_1#18: 4 f(1,2,3)#2 + 4 f(1,3,2)#3 + c(1,1)#9
                             + 2 c(1,2)#11 + 3 c(1,3)#13 + 4 c(1,4)#15
                             - maxSlice(1)#17 - RgMaxUsedSlicePerLink_2_1#18
                              = -4
 MaxUsedSlicePerLink_1_1#19: 4 f(1,1,2)#6 + 4 f(1,2,1)#7 + c(1,1)#9
                             + 2 c(1,2)#11 + 3 c(1,3)#13 + 4 c(1,4)#15
                             - maxSlice(0)#18 - RgMaxUsedSlicePerLink_1_1#19
                              = -4
 MaxUsedSliceOverall_1#20:   c(1,1)#8 + 2 c(1,2)#10 + 3 c(1,3)#12 + 4 c(1,4)#14
                             - maxSliceOverall#19 - RgMaxUsedSliceOverall_1#20
                              = 0
 MaxUsedSliceOverall_1#21:   c(1,1)#9 + 2 c(1,2)#11 + 3 c(1,3)#13 + 4 c(1,4)#15
                             - maxSliceOverall#19 - RgMaxUsedSliceOverall_1#21
                              = 0
 Slice_1#22:                 c(1,1)#8  = 0
 Slice_1#23:                 c(1,1)#9  = 0
 NonOverlapping_11_1_1#24:   2 f(1,2,3)#0 + 2 f(1,2,3)#2 + 2 f(1,3,2)#3
                             - c(1,1)#8 + c(1,1)#9 - 2 c(1,2)#10 + 2 c(1,2)#11
                             - 3 c(1,3)#12 + 3 c(1,3)#13 - 4 c(1,4)#14
                             + 4 c(1,4)#15 - 4 z(1,1)#16
                             - RgNonOverlapping_11_1_1#24  = 0
 NonOverlapping_21_1_1#25:   2 f(1,2,3)#0 - 2 f(1,3,2)#1 + 2 f(1,2,3)#2
                             + 2 f(1,3,2)#3 + c(1,1)#8 - c(1,1)#9 + 2 c(1,2)#10
                             - 2 c(1,2)#11 + 3 c(1,3)#12 - 3 c(1,3)#13
                             + 4 c(1,4)#14 - 4 c(1,4)#15 - 4 z(1,1)#16
                             - RgNonOverlapping_21_1_1#25  = 0
 NonOverlapping_11_1_0#26:   - 2 f(1,3,2)#1 + 2 f(1,1,2)#4 + 2 f(1,2,1)#5
                             + 2 f(1,1,2)#6 + 2 f(1,2,1)#7 - c(1,1)#8
                             + c(1,1)#9 - 2 c(1,2)#10 + 2 c(1,2)#11
                             - 3 c(1,3)#12 + 3 c(1,3)#13 - 4 c(1,4)#14
                             + 4 c(1,4)#15 - 4 z(1,1)#16
                             - RgNonOverlapping_11_1_0#26  = 0
 NonOverlapping_21_1_0#27:   - 4 f(1,3,2)#1 + 2 f(1,1,2)#4 + 2 f(1,2,1)#5
                             + 2 f(1,1,2)#6 + 2 f(1,2,1)#7 + c(1,1)#8
                             - c(1,1)#9 + 2 c(1,2)#10 - 2 c(1,2)#11
                             + 3 c(1,3)#12 - 3 c(1,3)#13 + 4 c(1,4)#14
                             - 4 c(1,4)#15 - 4 z(1,1)#16
                             - RgNonOverlapping_21_1_0#27  = 0
 CBandRouting1_1#28:         c(1,1)#8 + c(1,2)#10 + c(1,3)#12 + c(1,4)#14
                             - C(1)#20  = 0
 CBandRouting2_1#29:         f(1,2,3)#0 + f(1,3,2)#1 - C(1)#20
                             - RgCBandRouting2_1#29  = 0
 CBandRouting2_1#30:         f(1,1,2)#4 + f(1,2,1)#5 - C(1)#20
                             - RgCBandRouting2_1#30  = 0
 CBandRouting1_1#31:         c(1,1)#9 + c(1,2)#11 + c(1,3)#13 + c(1,4)#15
                             - C(1)#21  = 0
 CBandRouting2_1#32:         f(1,2,3)#2 + f(1,3,2)#3 - C(1)#21
                             - RgCBandRouting2_1#32  = 0
 CBandRouting2_1#33:         f(1,1,2)#6 + f(1,2,1)#7 - C(1)#21
                             - RgCBandRouting2_1#33  = 0
 Prepro_1#34:                f(1,3,2)#1  = 0
 Prepro_1#35:                 = 0
Bounds
 0 <= f(1,2,3)#0 <= 1
 0 <= f(1,3,2)#1 <= 1
 0 <= f(1,2,3)#2 <= 1
 0 <= f(1,3,2)#3 <= 1
 0 <= f(1,1,2)#4 <= 1
 0 <= f(1,2,1)#5 <= 1
 0 <= f(1,1,2)#6 <= 1
 0 <= f(1,2,1)#7 <= 1
 0 <= c(1,1)#8 <= 1
 0 <= c(1,1)#9 <= 1
 0 <= c(1,2)#10 <= 1
 0 <= c(1,2)#11 <= 1
 0 <= c(1,3)#12 <= 1
 0 <= c(1,3)#13 <= 1
 0 <= c(1,4)#14 <= 1
 0 <= c(1,4)#15 <= 1
 0 <= z(1,1)#16 <= 1
 0 <= maxSlice(1)#17 <= 4
 0 <= maxSlice(0)#18 <= 4
 0 <= maxSliceOverall#19 <= 4
 0 <= C(1)#20 <= 1
 0 <= C(1)#21 <= 1
 0 <= RgSource_1#0 <= 1
 0 <= RgSource2_1#1 <= 1
 0 <= RgSource_1#3 <= 1
 0 <= RgSource2_1#4 <= 1
 0 <= RgTarget2_1#7 <= 1
 0 <= RgTarget2_1#9 <= 1
 0 <= RgDegree_1_3#10 <= 1
 0 <= RgDegree_1_2#11 <= 1
 0 <= RgLength_1#14 <= 2000
 0 <= RgLength_1#15 <= 2000
 0 <= RgMaxUsedSlicePerLink_2_1#16 <= 8
 0 <= RgMaxUsedSlicePerLink_1_1#17 <= 8
 0 <= RgMaxUsedSlicePerLink_2_1#18 <= 8
 0 <= RgMaxUsedSlicePerLink_1_1#19 <= 8
-4 <= RgMaxUsedSliceOverall_1#20 <= 0
-4 <= RgMaxUsedSliceOverall_1#21 <= 0
-2147483648 <= RgNonOverlapping_11_1_1#24 <= 0
-2147483648 <= RgNonOverlapping_21_1_1#25 <= 0
-2147483648 <= RgNonOverlapping_11_1_0#26 <= 0
-2147483648 <= RgNonOverlapping_21_1_0#27 <= 0
-1 <= RgCBandRouting2_1#29 <= 0
-1 <= RgCBandRouting2_1#30 <= 0
-1 <= RgCBandRouting2_1#32 <= 0
-1 <= RgCBandRouting2_1#33 <= 0
Binaries
 f(1,2,3)#0  f(1,3,2)#1  f(1,2,3)#2  f(1,3,2)#3  f(1,1,2)#4  f(1,2,1)#5 
 f(1,1,2)#6  f(1,2,1)#7  c(1,1)#8  c(1,1)#9  c(1,2)#10  c(1,2)#11  c(1,3)#12 
 c(1,3)#13  c(1,4)#14  c(1,4)#15  z(1,1)#16  C(1)#20  C(1)#21 
Generals
 maxSlice(1)#17  maxSlice(0)#18  maxSliceOverall#19 
End
