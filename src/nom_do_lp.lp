\ENCODING=ISO-8859-1
\Problem name: IloCplex

Minimize
 obj_10p: - 2 C(1) - 2 C(2)
Subject To
 Source_1#0:                 c(1,1) + c(1,2) + c(1,3) - RgSource_1#0  = 0
 Source2_1#1:                f(1,1,2) - c(1,1) - c(1,2) - c(1,3)
                             - RgSource2_1#1  = 0
 Source3_1#2:                f(1,2,1)  = 0
 Source_2#3:                 c(2,1) + c(2,2) + c(2,3) - RgSource_2#3  = 0
 Source2_2#4:                f(2,1,2) - c(2,1) - c(2,2) - c(2,3)
                             - RgSource2_2#4  = 0
 Source3_2#5:                f(2,2,1)  = 0
 Target_1#6:                 f(1,2,3) + f(1,2,1)  = 0
 Target2_1#7:                f(1,3,2) + f(1,1,2) - RgTarget2_1#7  = 0
 Target_2#8:                 f(2,3,2)  = 0
 Target2_2#9:                f(2,2,3) - RgTarget2_2#9  = 0
 Degree_1_3#10:              f(1,3,2) - RgDegree_1_3#10  = 0
 Degree_2_2#11:              f(2,2,3) + f(2,2,1) - RgDegree_2_2#11  = 0
 Flow_3_1#12:                - f(1,2,3) + f(1,3,2)  = 0
 Flow_2_2#13:                f(2,2,3) - f(2,3,2) - f(2,1,2) + f(2,2,1)  = 0
 Length_1#14:                100 f(1,2,3) + 100 f(1,3,2) + 100 f(1,1,2)
                             + 100 f(1,2,1) - RgLength_1#14  = 0
 Length_2#15:                100 f(2,2,3) + 100 f(2,3,2) + 100 f(2,1,2)
                             + 100 f(2,2,1) - RgLength_2#15  = 0
 MaxUsedSlicePerLink_2_1#16: 3 f(1,2,3) + 3 f(1,3,2) + c(1,1) + 2 c(1,2)
                             + 3 c(1,3) - maxSlice(1)
                             - RgMaxUsedSlicePerLink_2_1#16  = -3
 MaxUsedSlicePerLink_1_1#17: 3 f(1,1,2) + 3 f(1,2,1) + c(1,1) + 2 c(1,2)
                             + 3 c(1,3) - maxSlice(0)
                             - RgMaxUsedSlicePerLink_1_1#17  = -3
 MaxUsedSlicePerLink_2_2#18: 3 f(2,2,3) + 3 f(2,3,2) + c(2,1) + 2 c(2,2)
                             + 3 c(2,3) - maxSlice(1)
                             - RgMaxUsedSlicePerLink_2_2#18  = -3
 MaxUsedSlicePerLink_1_2#19: 3 f(2,1,2) + 3 f(2,2,1) + c(2,1) + 2 c(2,2)
                             + 3 c(2,3) - maxSlice(0)
                             - RgMaxUsedSlicePerLink_1_2#19  = -3
 MaxUsedSliceOverall_1#20:   c(1,1) + 2 c(1,2) + 3 c(1,3) - maxSliceOverall
                             - RgMaxUsedSliceOverall_1#20  = 0
 MaxUsedSliceOverall_2#21:   c(2,1) + 2 c(2,2) + 3 c(2,3) - maxSliceOverall
                             - RgMaxUsedSliceOverall_2#21  = 0
 Slice_1#22:                 c(1,1)  = 0
 Slice_2#23:                 c(2,1)  = 0
 NonOverlapping_11_2_1#24:   2 f(1,2,3) + 2 f(1,3,2) + 2 f(2,2,3) + 2 f(2,3,2)
                             - c(1,1) + c(2,1) - 2 c(1,2) + 2 c(2,2) - 3 c(1,3)
                             + 3 c(2,3) - 3 z(1,2) - RgNonOverlapping_11_2_1#24
                              = 2
 NonOverlapping_21_2_1#25:   2 f(1,2,3) + 2 f(1,3,2) + 2 f(2,2,3) + 2 f(2,3,2)
                             + c(1,1) - c(2,1) + 2 c(1,2) - 2 c(2,2) + 3 c(1,3)
                             - 3 c(2,3) + 3 z(1,2) - RgNonOverlapping_21_2_1#25
                              = 5
 NonOverlapping_11_2_0#26:   2 f(1,1,2) + 2 f(1,2,1) + 2 f(2,1,2) + 2 f(2,2,1)
                             - c(1,1) + c(2,1) - 2 c(1,2) + 2 c(2,2) - 3 c(1,3)
                             + 3 c(2,3) - 3 z(1,2) - RgNonOverlapping_11_2_0#26
                              = 2
 NonOverlapping_21_2_0#27:   2 f(1,1,2) + 2 f(1,2,1) + 2 f(2,1,2) + 2 f(2,2,1)
                             + c(1,1) - c(2,1) + 2 c(1,2) - 2 c(2,2) + 3 c(1,3)
                             - 3 c(2,3) + 3 z(1,2) - RgNonOverlapping_21_2_0#27
                              = 5
 CBandRouting1_1#28:         c(1,1) + c(1,2) + c(1,3) - C(1)  = 0
 CBandRouting2_1#29:         f(1,2,3) + f(1,3,2) - C(1) - RgCBandRouting2_1#29
                              = 0
 CBandRouting2_1#30:         f(1,1,2) + f(1,2,1) - C(1) - RgCBandRouting2_1#30
                              = 0
 CBandRouting1_2#31:         c(2,1) + c(2,2) + c(2,3) - C(2)  = 0
 CBandRouting2_2#32:         f(2,2,3) + f(2,3,2) - C(2) - RgCBandRouting2_2#32
                              = 0
 CBandRouting2_2#33:         f(2,1,2) + f(2,2,1) - C(2) - RgCBandRouting2_2#33
                              = 0
Bounds
 0 <= f(1,2,3) <= 1
 0 <= f(1,3,2) <= 1
 0 <= f(2,2,3) <= 1
 0 <= f(2,3,2) <= 1
 0 <= f(1,1,2) <= 1
 0 <= f(1,2,1) <= 1
 0 <= f(2,1,2) <= 1
 0 <= f(2,2,1) <= 1
 0 <= c(1,1) <= 1
 0 <= c(2,1) <= 1
 0 <= c(1,2) <= 1
 0 <= c(2,2) <= 1
 0 <= c(1,3) <= 1
 0 <= c(2,3) <= 1
 0 <= z(1,2) <= 1
 0 <= maxSlice(1) <= 3
 0 <= maxSlice(0) <= 3
 0 <= maxSliceOverall <= 3
 0 <= C(1) <= 1
 0 <= C(2) <= 1
 0 <= RgSource_1#0 <= 1
 0 <= RgSource2_1#1 <= 1
 0 <= RgSource_2#3 <= 1
 0 <= RgSource2_2#4 <= 1
 0 <= RgTarget2_1#7 <= 1
 0 <= RgTarget2_2#9 <= 1
 0 <= RgDegree_1_3#10 <= 1
 0 <= RgDegree_2_2#11 <= 1
 0 <= RgLength_1#14 <= 2000
 0 <= RgLength_2#15 <= 2000
 0 <= RgMaxUsedSlicePerLink_2_1#16 <= 6
 0 <= RgMaxUsedSlicePerLink_1_1#17 <= 6
 0 <= RgMaxUsedSlicePerLink_2_2#18 <= 6
 0 <= RgMaxUsedSlicePerLink_1_2#19 <= 6
-3 <= RgMaxUsedSliceOverall_1#20 <= 0
-3 <= RgMaxUsedSliceOverall_2#21 <= 0
-8 <= RgNonOverlapping_11_2_1#24 <= 0
-11 <= RgNonOverlapping_21_2_1#25 <= 0
-8 <= RgNonOverlapping_11_2_0#26 <= 0
-11 <= RgNonOverlapping_21_2_0#27 <= 0
-1 <= RgCBandRouting2_1#29 <= 0
-1 <= RgCBandRouting2_1#30 <= 0
-1 <= RgCBandRouting2_2#32 <= 0
-1 <= RgCBandRouting2_2#33 <= 0
Binaries
 f(1,2,3)  f(1,3,2)  f(2,2,3)  f(2,3,2)  f(1,1,2)  f(1,2,1)  f(2,1,2) 
 f(2,2,1)  c(1,1)  c(2,1)  c(1,2)  c(2,2)  c(1,3)  c(2,3)  z(1,2)  C(1)  C(2) 
Generals
 maxSlice(1)  maxSlice(0)  maxSliceOverall 
End
