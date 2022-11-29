\ENCODING=ISO-8859-1
\Problem name: IloCplex

Minimize
 obj_2: x(1,3,4,7) + x(1,3,4,6) + x(1,3,4,5) + x(1,3,4,4) + x(1,2,4,7)
        + x(1,1,2,4) + x(1,1,3,4) + x(1,1,4,4) + x(1,1,2,5) + x(1,1,3,5)
        + x(1,1,4,5) + x(1,1,2,6) + x(1,1,3,6) + x(1,1,4,6) + x(1,1,4,7)
        + x(1,1,3,7) + x(1,1,2,7) + x(1,2,4,6) + x(1,2,4,5) + x(1,2,4,4)
        + x(2,3,4,7) + x(2,3,4,6) + x(2,3,4,5) + x(2,3,4,4) + x(2,2,4,7)
        + x(2,1,2,4) + x(2,1,3,4) + x(2,1,4,4) + x(2,1,2,5) + x(2,1,3,5)
        + x(2,1,4,5) + x(2,1,2,6) + x(2,1,3,6) + x(2,1,4,6) + x(2,1,4,7)
        + x(2,1,3,7) + x(2,1,2,7) + x(2,2,4,6) + x(2,2,4,5) + x(2,2,4,4)
Subject To
 Source_1_1:               x(1,1,2,4) + x(1,1,3,4) + x(1,1,4,4) + x(1,1,2,5)
                           + x(1,1,3,5) + x(1,1,4,5) + x(1,1,2,6) + x(1,1,3,6)
                           + x(1,1,4,6) + x(1,1,4,7) + x(1,1,3,7) + x(1,1,2,7)
                            = 1
 Source_2_1:               x(1,2,4,7) + x(1,2,4,6) + x(1,2,4,5) + x(1,2,4,4)
                           - RgSource_2_1  = 0
 Source_3_1:               x(1,3,4,7) + x(1,3,4,6) + x(1,3,4,5) + x(1,3,4,4)
                           - RgSource_3_1  = 0
 Source_4_1:                = 0
 Source_1_2:               x(2,1,2,4) + x(2,1,3,4) + x(2,1,4,4) + x(2,1,2,5)
                           + x(2,1,3,5) + x(2,1,4,5) + x(2,1,2,6) + x(2,1,3,6)
                           + x(2,1,4,6) + x(2,1,4,7) + x(2,1,3,7) + x(2,1,2,7)
                            = 1
 Source_2_2:               x(2,2,4,7) + x(2,2,4,6) + x(2,2,4,5) + x(2,2,4,4)
                           - RgSource_2_2  = 0
 Source_3_2:               x(2,3,4,7) + x(2,3,4,6) + x(2,3,4,5) + x(2,3,4,4)
                           - RgSource_3_2  = 0
 Source_4_2:                = 0
 Flow_3_7_1:               x(1,3,4,7) - x(1,1,3,7)  = 0
 Flow_3_6_1:               x(1,3,4,6) - x(1,1,3,6)  = 0
 Flow_3_5_1:               x(1,3,4,5) - x(1,1,3,5)  = 0
 Flow_3_4_1:               x(1,3,4,4) - x(1,1,3,4)  = 0
 Flow_3_3_1:                = 0
 Flow_3_2_1:                = 0
 Flow_3_1_1:                = 0
 Flow_2_7_1:               x(1,2,4,7) - x(1,1,2,7)  = 0
 Flow_2_6_1:               - x(1,1,2,6) + x(1,2,4,6)  = 0
 Flow_2_5_1:               - x(1,1,2,5) + x(1,2,4,5)  = 0
 Flow_2_4_1:               - x(1,1,2,4) + x(1,2,4,4)  = 0
 Flow_2_3_1:                = 0
 Flow_2_2_1:                = 0
 Flow_2_1_1:                = 0
 Flow_3_7_2:               x(2,3,4,7) - x(2,1,3,7)  = 0
 Flow_3_6_2:               x(2,3,4,6) - x(2,1,3,6)  = 0
 Flow_3_5_2:               x(2,3,4,5) - x(2,1,3,5)  = 0
 Flow_3_4_2:               x(2,3,4,4) - x(2,1,3,4)  = 0
 Flow_3_3_2:                = 0
 Flow_3_2_2:                = 0
 Flow_3_1_2:                = 0
 Flow_2_7_2:               x(2,2,4,7) - x(2,1,2,7)  = 0
 Flow_2_6_2:               - x(2,1,2,6) + x(2,2,4,6)  = 0
 Flow_2_5_2:               - x(2,1,2,5) + x(2,2,4,5)  = 0
 Flow_2_4_2:               - x(2,1,2,4) + x(2,2,4,4)  = 0
 Flow_2_3_2:                = 0
 Flow_2_2_2:                = 0
 Flow_2_1_2:                = 0
 Target_1:                 x(1,3,4,7) + x(1,3,4,6) + x(1,3,4,5) + x(1,3,4,4)
                           + x(1,2,4,7) + x(1,1,4,4) + x(1,1,4,5) + x(1,1,4,6)
                           + x(1,1,4,7) + x(1,2,4,6) + x(1,2,4,5) + x(1,2,4,4)
                            = 1
 Target_2:                 x(2,3,4,7) + x(2,3,4,6) + x(2,3,4,5) + x(2,3,4,4)
                           + x(2,2,4,7) + x(2,1,4,4) + x(2,1,4,5) + x(2,1,4,6)
                           + x(2,1,4,7) + x(2,2,4,6) + x(2,2,4,5) + x(2,2,4,4)
                            = 1
 OSNR_1:                   2 x(1,3,4,7) + 2 x(1,3,4,6) + 2 x(1,3,4,5)
                           + 2 x(1,3,4,4) + 2 x(1,2,4,7) + 2 x(1,1,2,4)
                           + 2 x(1,1,3,4) + 2 x(1,1,4,4) + 2 x(1,1,2,5)
                           + 2 x(1,1,3,5) + 2 x(1,1,4,5) + 2 x(1,1,2,6)
                           + 2 x(1,1,3,6) + 2 x(1,1,4,6) + 2 x(1,1,4,7)
                           + 2 x(1,1,3,7) + 2 x(1,1,2,7) + 2 x(1,2,4,6)
                           + 2 x(1,2,4,5) + 2 x(1,2,4,4) - RgOSNR_1  = 0
 OSNR_2:                   2 x(2,3,4,7) + 2 x(2,3,4,6) + 2 x(2,3,4,5)
                           + 2 x(2,3,4,4) + 2 x(2,2,4,7) + 2 x(2,1,2,4)
                           + 2 x(2,1,3,4) + 2 x(2,1,4,4) + 2 x(2,1,2,5)
                           + 2 x(2,1,3,5) + 2 x(2,1,4,5) + 2 x(2,1,2,6)
                           + 2 x(2,1,3,6) + 2 x(2,1,4,6) + 2 x(2,1,4,7)
                           + 2 x(2,1,3,7) + 2 x(2,1,2,7) + 2 x(2,2,4,6)
                           + 2 x(2,2,4,5) + 2 x(2,2,4,4) - RgOSNR_2  = 0
 Length_1:                 200 x(1,3,4,7) + 200 x(1,3,4,6) + 200 x(1,3,4,5)
                           + 200 x(1,3,4,4) + 200 x(1,2,4,7) + 200 x(1,1,2,4)
                           + 200 x(1,1,3,4) + 200 x(1,1,4,4) + 200 x(1,1,2,5)
                           + 200 x(1,1,3,5) + 200 x(1,1,4,5) + 200 x(1,1,2,6)
                           + 200 x(1,1,3,6) + 200 x(1,1,4,6) + 200 x(1,1,4,7)
                           + 200 x(1,1,3,7) + 200 x(1,1,2,7) + 200 x(1,2,4,6)
                           + 200 x(1,2,4,5) + 200 x(1,2,4,4) - RgLength_1  = 0
 Length_2:                 200 x(2,3,4,7) + 200 x(2,3,4,6) + 200 x(2,3,4,5)
                           + 200 x(2,3,4,4) + 200 x(2,2,4,7) + 200 x(2,1,2,4)
                           + 200 x(2,1,3,4) + 200 x(2,1,4,4) + 200 x(2,1,2,5)
                           + 200 x(2,1,3,5) + 200 x(2,1,4,5) + 200 x(2,1,2,6)
                           + 200 x(2,1,3,6) + 200 x(2,1,4,6) + 200 x(2,1,4,7)
                           + 200 x(2,1,3,7) + 200 x(2,1,2,7) + 200 x(2,2,4,6)
                           + 200 x(2,2,4,5) + 200 x(2,2,4,4) - RgLength_2  = 0
 NonOverlap_1_1:           x(1,1,2,4) + x(2,1,2,4) - RgNonOverlap_1_1  = 0
 NonOverlap_1_2:           x(1,1,2,4) + x(1,1,2,5) + x(2,1,2,4) + x(2,1,2,5)
                           - RgNonOverlap_1_2  = 0
 NonOverlap_1_3:           x(1,1,2,4) + x(1,1,2,5) + x(1,1,2,6) + x(2,1,2,4)
                           + x(2,1,2,5) + x(2,1,2,6) - RgNonOverlap_1_3  = 0
 NonOverlap_1_4:           x(1,1,2,4) + x(1,1,2,5) + x(1,1,2,6) + x(1,1,2,7)
                           + x(2,1,2,4) + x(2,1,2,5) + x(2,1,2,6) + x(2,1,2,7)
                           - RgNonOverlap_1_4  = 0
 NonOverlap_1_5:           x(1,1,2,5) + x(1,1,2,6) + x(1,1,2,7) + x(2,1,2,5)
                           + x(2,1,2,6) + x(2,1,2,7) - RgNonOverlap_1_5  = 0
 NonOverlap_1_6:           x(1,1,2,6) + x(1,1,2,7) + x(2,1,2,6) + x(2,1,2,7)
                           - RgNonOverlap_1_6  = 0
 NonOverlap_1_7:           x(1,1,2,7) + x(2,1,2,7) - RgNonOverlap_1_7  = 0
 NonOverlap_2_1:           x(1,1,3,4) + x(2,1,3,4) - RgNonOverlap_2_1  = 0
 NonOverlap_2_2:           x(1,1,3,4) + x(1,1,3,5) + x(2,1,3,4) + x(2,1,3,5)
                           - RgNonOverlap_2_2  = 0
 NonOverlap_2_3:           x(1,1,3,4) + x(1,1,3,5) + x(1,1,3,6) + x(2,1,3,4)
                           + x(2,1,3,5) + x(2,1,3,6) - RgNonOverlap_2_3  = 0
 NonOverlap_2_4:           x(1,1,3,4) + x(1,1,3,5) + x(1,1,3,6) + x(1,1,3,7)
                           + x(2,1,3,4) + x(2,1,3,5) + x(2,1,3,6) + x(2,1,3,7)
                           - RgNonOverlap_2_4  = 0
 NonOverlap_2_5:           x(1,1,3,5) + x(1,1,3,6) + x(1,1,3,7) + x(2,1,3,5)
                           + x(2,1,3,6) + x(2,1,3,7) - RgNonOverlap_2_5  = 0
 NonOverlap_2_6:           x(1,1,3,6) + x(1,1,3,7) + x(2,1,3,6) + x(2,1,3,7)
                           - RgNonOverlap_2_6  = 0
 NonOverlap_2_7:           x(1,1,3,7) + x(2,1,3,7) - RgNonOverlap_2_7  = 0
 NonOverlap_3_1:           x(1,1,4,4) + x(2,1,4,4) - RgNonOverlap_3_1  = 0
 NonOverlap_3_2:           x(1,1,4,4) + x(1,1,4,5) + x(2,1,4,4) + x(2,1,4,5)
                           - RgNonOverlap_3_2  = 0
 NonOverlap_3_3:           x(1,1,4,4) + x(1,1,4,5) + x(1,1,4,6) + x(2,1,4,4)
                           + x(2,1,4,5) + x(2,1,4,6) - RgNonOverlap_3_3  = 0
 NonOverlap_3_4:           x(1,1,4,4) + x(1,1,4,5) + x(1,1,4,6) + x(1,1,4,7)
                           + x(2,1,4,4) + x(2,1,4,5) + x(2,1,4,6) + x(2,1,4,7)
                           - RgNonOverlap_3_4  = 0
 NonOverlap_3_5:           x(1,1,4,5) + x(1,1,4,6) + x(1,1,4,7) + x(2,1,4,5)
                           + x(2,1,4,6) + x(2,1,4,7) - RgNonOverlap_3_5  = 0
 NonOverlap_3_6:           x(1,1,4,6) + x(1,1,4,7) + x(2,1,4,6) + x(2,1,4,7)
                           - RgNonOverlap_3_6  = 0
 NonOverlap_3_7:           x(1,1,4,7) + x(2,1,4,7) - RgNonOverlap_3_7  = 0
 NonOverlap_4_1:           x(1,2,4,4) + x(2,2,4,4) - RgNonOverlap_4_1  = 0
 NonOverlap_4_2:           x(1,2,4,5) + x(1,2,4,4) + x(2,2,4,5) + x(2,2,4,4)
                           - RgNonOverlap_4_2  = 0
 NonOverlap_4_3:           x(1,2,4,6) + x(1,2,4,5) + x(1,2,4,4) + x(2,2,4,6)
                           + x(2,2,4,5) + x(2,2,4,4) - RgNonOverlap_4_3  = 0
 NonOverlap_4_4:           x(1,2,4,7) + x(1,2,4,6) + x(1,2,4,5) + x(1,2,4,4)
                           + x(2,2,4,7) + x(2,2,4,6) + x(2,2,4,5) + x(2,2,4,4)
                           - RgNonOverlap_4_4  = 0
 NonOverlap_4_5:           x(1,2,4,7) + x(1,2,4,6) + x(1,2,4,5) + x(2,2,4,7)
                           + x(2,2,4,6) + x(2,2,4,5) - RgNonOverlap_4_5  = 0
 NonOverlap_4_6:           x(1,2,4,7) + x(1,2,4,6) + x(2,2,4,7) + x(2,2,4,6)
                           - RgNonOverlap_4_6  = 0
 NonOverlap_4_7:           x(1,2,4,7) + x(2,2,4,7) - RgNonOverlap_4_7  = 0
 NonOverlap_5_1:           x(1,3,4,4) + x(2,3,4,4) - RgNonOverlap_5_1  = 0
 NonOverlap_5_2:           x(1,3,4,5) + x(1,3,4,4) + x(2,3,4,5) + x(2,3,4,4)
                           - RgNonOverlap_5_2  = 0
 NonOverlap_5_3:           x(1,3,4,6) + x(1,3,4,5) + x(1,3,4,4) + x(2,3,4,6)
                           + x(2,3,4,5) + x(2,3,4,4) - RgNonOverlap_5_3  = 0
 NonOverlap_5_4:           x(1,3,4,7) + x(1,3,4,6) + x(1,3,4,5) + x(1,3,4,4)
                           + x(2,3,4,7) + x(2,3,4,6) + x(2,3,4,5) + x(2,3,4,4)
                           - RgNonOverlap_5_4  = 0
 NonOverlap_5_5:           x(1,3,4,7) + x(1,3,4,6) + x(1,3,4,5) + x(2,3,4,7)
                           + x(2,3,4,6) + x(2,3,4,5) - RgNonOverlap_5_5  = 0
 NonOverlap_5_6:           x(1,3,4,7) + x(1,3,4,6) + x(2,3,4,7) + x(2,3,4,6)
                           - RgNonOverlap_5_6  = 0
 NonOverlap_5_7:           x(1,3,4,7) + x(2,3,4,7) - RgNonOverlap_5_7  = 0
 MaxUsedSlicePerLink_1_1:  3 x(1,1,2,4) + 4 x(1,1,2,5) + 5 x(1,1,2,6)
                           + 6 x(1,1,2,7) - maxSlice(1)
                           - RgMaxUsedSlicePerLink_1_1  = 0
 MaxUsedSlicePerLink_1_2:  3 x(2,1,2,4) + 4 x(2,1,2,5) + 5 x(2,1,2,6)
                           + 6 x(2,1,2,7) - maxSlice(1)
                           - RgMaxUsedSlicePerLink_1_2  = 0
 MaxUsedSlicePerLink_2_1:  3 x(1,1,3,4) + 4 x(1,1,3,5) + 5 x(1,1,3,6)
                           + 6 x(1,1,3,7) - maxSlice(2)
                           - RgMaxUsedSlicePerLink_2_1  = 0
 MaxUsedSlicePerLink_2_2:  3 x(2,1,3,4) + 4 x(2,1,3,5) + 5 x(2,1,3,6)
                           + 6 x(2,1,3,7) - maxSlice(2)
                           - RgMaxUsedSlicePerLink_2_2  = 0
 MaxUsedSlicePerLink_3_1:  3 x(1,1,4,4) + 4 x(1,1,4,5) + 5 x(1,1,4,6)
                           + 6 x(1,1,4,7) - maxSlice(3)
                           - RgMaxUsedSlicePerLink_3_1  = 0
 MaxUsedSlicePerLink_3_2:  3 x(2,1,4,4) + 4 x(2,1,4,5) + 5 x(2,1,4,6)
                           + 6 x(2,1,4,7) - maxSlice(3)
                           - RgMaxUsedSlicePerLink_3_2  = 0
 MaxUsedSlicePerLink_4_1:  6 x(1,2,4,7) + 5 x(1,2,4,6) + 4 x(1,2,4,5)
                           + 3 x(1,2,4,4) - maxSlice(4)
                           - RgMaxUsedSlicePerLink_4_1  = 0
 MaxUsedSlicePerLink_4_2:  6 x(2,2,4,7) + 5 x(2,2,4,6) + 4 x(2,2,4,5)
                           + 3 x(2,2,4,4) - maxSlice(4)
                           - RgMaxUsedSlicePerLink_4_2  = 0
 MaxUsedSlicePerLink_5_1:  6 x(1,3,4,7) + 5 x(1,3,4,6) + 4 x(1,3,4,5)
                           + 3 x(1,3,4,4) - maxSlice(5)
                           - RgMaxUsedSlicePerLink_5_1  = 0
 MaxUsedSlicePerLink_5_2:  6 x(2,3,4,7) + 5 x(2,3,4,6) + 4 x(2,3,4,5)
                           + 3 x(2,3,4,4) - maxSlice(5)
                           - RgMaxUsedSlicePerLink_5_2  = 0
 MaxUsedSliceOverall_1:    3 x(1,1,2,4) + 3 x(1,1,3,4) + 3 x(1,1,4,4)
                           + 4 x(1,1,2,5) + 4 x(1,1,3,5) + 4 x(1,1,4,5)
                           + 5 x(1,1,2,6) + 5 x(1,1,3,6) + 5 x(1,1,4,6)
                           + 6 x(1,1,4,7) + 6 x(1,1,3,7) + 6 x(1,1,2,7)
                           - maxSliceOverall - RgMaxUsedSliceOverall_1  = 0
 MaxUsedSliceOverall_2:    3 x(2,1,2,4) + 3 x(2,1,3,4) + 3 x(2,1,4,4)
                           + 4 x(2,1,2,5) + 4 x(2,1,3,5) + 4 x(2,1,4,5)
                           + 5 x(2,1,2,6) + 5 x(2,1,3,6) + 5 x(2,1,4,6)
                           + 6 x(2,1,4,7) + 6 x(2,1,3,7) + 6 x(2,1,2,7)
                           - maxSliceOverall - RgMaxUsedSliceOverall_2  = 0
 MaxUsedSliceOverall2_1_1: 3 x(1,1,2,4) + 3 x(2,1,2,4) - maxSliceOverall
                           - RgMaxUsedSliceOverall2_1_1  = 0
 MaxUsedSliceOverall2_1_2: 3 x(1,1,2,4) + 4 x(1,1,2,5) + 3 x(2,1,2,4)
                           + 4 x(2,1,2,5) - maxSliceOverall
                           - RgMaxUsedSliceOverall2_1_2  = 0
 MaxUsedSliceOverall2_1_3: 3 x(1,1,2,4) + 4 x(1,1,2,5) + 5 x(1,1,2,6)
                           + 3 x(2,1,2,4) + 4 x(2,1,2,5) + 5 x(2,1,2,6)
                           - maxSliceOverall - RgMaxUsedSliceOverall2_1_3  = 0
 MaxUsedSliceOverall2_1_4: 3 x(1,1,2,4) + 4 x(1,1,2,5) + 5 x(1,1,2,6)
                           + 6 x(1,1,2,7) + 3 x(2,1,2,4) + 4 x(2,1,2,5)
                           + 5 x(2,1,2,6) + 6 x(2,1,2,7) - maxSliceOverall
                           - RgMaxUsedSliceOverall2_1_4  = 0
 MaxUsedSliceOverall2_1_5: 4 x(1,1,2,5) + 5 x(1,1,2,6) + 6 x(1,1,2,7)
                           + 4 x(2,1,2,5) + 5 x(2,1,2,6) + 6 x(2,1,2,7)
                           - maxSliceOverall - RgMaxUsedSliceOverall2_1_5  = 0
 MaxUsedSliceOverall2_1_6: 5 x(1,1,2,6) + 6 x(1,1,2,7) + 5 x(2,1,2,6)
                           + 6 x(2,1,2,7) - maxSliceOverall
                           - RgMaxUsedSliceOverall2_1_6  = 0
 MaxUsedSliceOverall2_1_7: 6 x(1,1,2,7) + 6 x(2,1,2,7) - maxSliceOverall
                           - RgMaxUsedSliceOverall2_1_7  = 0
 MaxUsedSliceOverall2_2_1: 3 x(1,1,3,4) + 3 x(2,1,3,4) - maxSliceOverall
                           - RgMaxUsedSliceOverall2_2_1  = 0
 MaxUsedSliceOverall2_2_2: 3 x(1,1,3,4) + 4 x(1,1,3,5) + 3 x(2,1,3,4)
                           + 4 x(2,1,3,5) - maxSliceOverall
                           - RgMaxUsedSliceOverall2_2_2  = 0
 MaxUsedSliceOverall2_2_3: 3 x(1,1,3,4) + 4 x(1,1,3,5) + 5 x(1,1,3,6)
                           + 3 x(2,1,3,4) + 4 x(2,1,3,5) + 5 x(2,1,3,6)
                           - maxSliceOverall - RgMaxUsedSliceOverall2_2_3  = 0
 MaxUsedSliceOverall2_2_4: 3 x(1,1,3,4) + 4 x(1,1,3,5) + 5 x(1,1,3,6)
                           + 6 x(1,1,3,7) + 3 x(2,1,3,4) + 4 x(2,1,3,5)
                           + 5 x(2,1,3,6) + 6 x(2,1,3,7) - maxSliceOverall
                           - RgMaxUsedSliceOverall2_2_4  = 0
 MaxUsedSliceOverall2_2_5: 4 x(1,1,3,5) + 5 x(1,1,3,6) + 6 x(1,1,3,7)
                           + 4 x(2,1,3,5) + 5 x(2,1,3,6) + 6 x(2,1,3,7)
                           - maxSliceOverall - RgMaxUsedSliceOverall2_2_5  = 0
 MaxUsedSliceOverall2_2_6: 5 x(1,1,3,6) + 6 x(1,1,3,7) + 5 x(2,1,3,6)
                           + 6 x(2,1,3,7) - maxSliceOverall
                           - RgMaxUsedSliceOverall2_2_6  = 0
 MaxUsedSliceOverall2_2_7: 6 x(1,1,3,7) + 6 x(2,1,3,7) - maxSliceOverall
                           - RgMaxUsedSliceOverall2_2_7  = 0
 MaxUsedSliceOverall2_3_1: 3 x(1,1,4,4) + 3 x(2,1,4,4) - maxSliceOverall
                           - RgMaxUsedSliceOverall2_3_1  = 0
 MaxUsedSliceOverall2_3_2: 3 x(1,1,4,4) + 4 x(1,1,4,5) + 3 x(2,1,4,4)
                           + 4 x(2,1,4,5) - maxSliceOverall
                           - RgMaxUsedSliceOverall2_3_2  = 0
 MaxUsedSliceOverall2_3_3: 3 x(1,1,4,4) + 4 x(1,1,4,5) + 5 x(1,1,4,6)
                           + 3 x(2,1,4,4) + 4 x(2,1,4,5) + 5 x(2,1,4,6)
                           - maxSliceOverall - RgMaxUsedSliceOverall2_3_3  = 0
 MaxUsedSliceOverall2_3_4: 3 x(1,1,4,4) + 4 x(1,1,4,5) + 5 x(1,1,4,6)
                           + 6 x(1,1,4,7) + 3 x(2,1,4,4) + 4 x(2,1,4,5)
                           + 5 x(2,1,4,6) + 6 x(2,1,4,7) - maxSliceOverall
                           - RgMaxUsedSliceOverall2_3_4  = 0
 MaxUsedSliceOverall2_3_5: 4 x(1,1,4,5) + 5 x(1,1,4,6) + 6 x(1,1,4,7)
                           + 4 x(2,1,4,5) + 5 x(2,1,4,6) + 6 x(2,1,4,7)
                           - maxSliceOverall - RgMaxUsedSliceOverall2_3_5  = 0
 MaxUsedSliceOverall2_3_6: 5 x(1,1,4,6) + 6 x(1,1,4,7) + 5 x(2,1,4,6)
                           + 6 x(2,1,4,7) - maxSliceOverall
                           - RgMaxUsedSliceOverall2_3_6  = 0
 MaxUsedSliceOverall2_3_7: 6 x(1,1,4,7) + 6 x(2,1,4,7) - maxSliceOverall
                           - RgMaxUsedSliceOverall2_3_7  = 0
 MaxUsedSliceOverall2_4_1: 3 x(1,2,4,4) + 3 x(2,2,4,4) - maxSliceOverall
                           - RgMaxUsedSliceOverall2_4_1  = 0
 MaxUsedSliceOverall2_4_2: 4 x(1,2,4,5) + 3 x(1,2,4,4) + 4 x(2,2,4,5)
                           + 3 x(2,2,4,4) - maxSliceOverall
                           - RgMaxUsedSliceOverall2_4_2  = 0
 MaxUsedSliceOverall2_4_3: 5 x(1,2,4,6) + 4 x(1,2,4,5) + 3 x(1,2,4,4)
                           + 5 x(2,2,4,6) + 4 x(2,2,4,5) + 3 x(2,2,4,4)
                           - maxSliceOverall - RgMaxUsedSliceOverall2_4_3  = 0
 MaxUsedSliceOverall2_4_4: 6 x(1,2,4,7) + 5 x(1,2,4,6) + 4 x(1,2,4,5)
                           + 3 x(1,2,4,4) + 6 x(2,2,4,7) + 5 x(2,2,4,6)
                           + 4 x(2,2,4,5) + 3 x(2,2,4,4) - maxSliceOverall
                           - RgMaxUsedSliceOverall2_4_4  = 0
 MaxUsedSliceOverall2_4_5: 6 x(1,2,4,7) + 5 x(1,2,4,6) + 4 x(1,2,4,5)
                           + 6 x(2,2,4,7) + 5 x(2,2,4,6) + 4 x(2,2,4,5)
                           - maxSliceOverall - RgMaxUsedSliceOverall2_4_5  = 0
 MaxUsedSliceOverall2_4_6: 6 x(1,2,4,7) + 5 x(1,2,4,6) + 6 x(2,2,4,7)
                           + 5 x(2,2,4,6) - maxSliceOverall
                           - RgMaxUsedSliceOverall2_4_6  = 0
 MaxUsedSliceOverall2_4_7: 6 x(1,2,4,7) + 6 x(2,2,4,7) - maxSliceOverall
                           - RgMaxUsedSliceOverall2_4_7  = 0
 MaxUsedSliceOverall2_5_1: 3 x(1,3,4,4) + 3 x(2,3,4,4) - maxSliceOverall
                           - RgMaxUsedSliceOverall2_5_1  = 0
 MaxUsedSliceOverall2_5_2: 4 x(1,3,4,5) + 3 x(1,3,4,4) + 4 x(2,3,4,5)
                           + 3 x(2,3,4,4) - maxSliceOverall
                           - RgMaxUsedSliceOverall2_5_2  = 0
 MaxUsedSliceOverall2_5_3: 5 x(1,3,4,6) + 4 x(1,3,4,5) + 3 x(1,3,4,4)
                           + 5 x(2,3,4,6) + 4 x(2,3,4,5) + 3 x(2,3,4,4)
                           - maxSliceOverall - RgMaxUsedSliceOverall2_5_3  = 0
 MaxUsedSliceOverall2_5_4: 6 x(1,3,4,7) + 5 x(1,3,4,6) + 4 x(1,3,4,5)
                           + 3 x(1,3,4,4) + 6 x(2,3,4,7) + 5 x(2,3,4,6)
                           + 4 x(2,3,4,5) + 3 x(2,3,4,4) - maxSliceOverall
                           - RgMaxUsedSliceOverall2_5_4  = 0
 MaxUsedSliceOverall2_5_5: 6 x(1,3,4,7) + 5 x(1,3,4,6) + 4 x(1,3,4,5)
                           + 6 x(2,3,4,7) + 5 x(2,3,4,6) + 4 x(2,3,4,5)
                           - maxSliceOverall - RgMaxUsedSliceOverall2_5_5  = 0
 MaxUsedSliceOverall2_5_6: 6 x(1,3,4,7) + 5 x(1,3,4,6) + 6 x(2,3,4,7)
                           + 5 x(2,3,4,6) - maxSliceOverall
                           - RgMaxUsedSliceOverall2_5_6  = 0
 MaxUsedSliceOverall2_5_7: 6 x(1,3,4,7) + 6 x(2,3,4,7) - maxSliceOverall
                           - RgMaxUsedSliceOverall2_5_7  = 0
 MaxUsedSliceOverall3_1_1: 3 x(1,1,2,4) + 3 x(1,1,3,4) + 3 x(1,1,4,4)
                           + 3 x(2,1,2,4) + 3 x(2,1,3,4) + 3 x(2,1,4,4)
                           - 3 maxSliceOverall - RgMaxUsedSliceOverall3_1_1
                            = 0
 MaxUsedSliceOverall3_1_2: 3 x(1,1,2,4) + 3 x(1,1,3,4) + 3 x(1,1,4,4)
                           + 4 x(1,1,2,5) + 4 x(1,1,3,5) + 4 x(1,1,4,5)
                           + 3 x(2,1,2,4) + 3 x(2,1,3,4) + 3 x(2,1,4,4)
                           + 4 x(2,1,2,5) + 4 x(2,1,3,5) + 4 x(2,1,4,5)
                           - 3 maxSliceOverall - RgMaxUsedSliceOverall3_1_2
                            = 0
 MaxUsedSliceOverall3_1_3: 3 x(1,1,2,4) + 3 x(1,1,3,4) + 3 x(1,1,4,4)
                           + 4 x(1,1,2,5) + 4 x(1,1,3,5) + 4 x(1,1,4,5)
                           + 5 x(1,1,2,6) + 5 x(1,1,3,6) + 5 x(1,1,4,6)
                           + 3 x(2,1,2,4) + 3 x(2,1,3,4) + 3 x(2,1,4,4)
                           + 4 x(2,1,2,5) + 4 x(2,1,3,5) + 4 x(2,1,4,5)
                           + 5 x(2,1,2,6) + 5 x(2,1,3,6) + 5 x(2,1,4,6)
                           - 3 maxSliceOverall - RgMaxUsedSliceOverall3_1_3
                            = 0
 MaxUsedSliceOverall3_1_4: 3 x(1,1,2,4) + 3 x(1,1,3,4) + 3 x(1,1,4,4)
                           + 4 x(1,1,2,5) + 4 x(1,1,3,5) + 4 x(1,1,4,5)
                           + 5 x(1,1,2,6) + 5 x(1,1,3,6) + 5 x(1,1,4,6)
                           + 6 x(1,1,4,7) + 6 x(1,1,3,7) + 6 x(1,1,2,7)
                           + 3 x(2,1,2,4) + 3 x(2,1,3,4) + 3 x(2,1,4,4)
                           + 4 x(2,1,2,5) + 4 x(2,1,3,5) + 4 x(2,1,4,5)
                           + 5 x(2,1,2,6) + 5 x(2,1,3,6) + 5 x(2,1,4,6)
                           + 6 x(2,1,4,7) + 6 x(2,1,3,7) + 6 x(2,1,2,7)
                           - 3 maxSliceOverall - RgMaxUsedSliceOverall3_1_4
                            = 0
 MaxUsedSliceOverall3_1_5: 4 x(1,1,2,5) + 4 x(1,1,3,5) + 4 x(1,1,4,5)
                           + 5 x(1,1,2,6) + 5 x(1,1,3,6) + 5 x(1,1,4,6)
                           + 6 x(1,1,4,7) + 6 x(1,1,3,7) + 6 x(1,1,2,7)
                           + 4 x(2,1,2,5) + 4 x(2,1,3,5) + 4 x(2,1,4,5)
                           + 5 x(2,1,2,6) + 5 x(2,1,3,6) + 5 x(2,1,4,6)
                           + 6 x(2,1,4,7) + 6 x(2,1,3,7) + 6 x(2,1,2,7)
                           - 3 maxSliceOverall - RgMaxUsedSliceOverall3_1_5
                            = 0
 MaxUsedSliceOverall3_1_6: 5 x(1,1,2,6) + 5 x(1,1,3,6) + 5 x(1,1,4,6)
                           + 6 x(1,1,4,7) + 6 x(1,1,3,7) + 6 x(1,1,2,7)
                           + 5 x(2,1,2,6) + 5 x(2,1,3,6) + 5 x(2,1,4,6)
                           + 6 x(2,1,4,7) + 6 x(2,1,3,7) + 6 x(2,1,2,7)
                           - 3 maxSliceOverall - RgMaxUsedSliceOverall3_1_6
                            = 0
 MaxUsedSliceOverall3_1_7: 6 x(1,1,4,7) + 6 x(1,1,3,7) + 6 x(1,1,2,7)
                           + 6 x(2,1,4,7) + 6 x(2,1,3,7) + 6 x(2,1,2,7)
                           - 3 maxSliceOverall - RgMaxUsedSliceOverall3_1_7
                            = 0
 MaxUsedSliceOverall3_2_1: 3 x(1,2,4,4) + 3 x(2,2,4,4) - 2 maxSliceOverall
                           - RgMaxUsedSliceOverall3_2_1  = 0
 MaxUsedSliceOverall3_2_2: 4 x(1,2,4,5) + 3 x(1,2,4,4) + 4 x(2,2,4,5)
                           + 3 x(2,2,4,4) - 2 maxSliceOverall
                           - RgMaxUsedSliceOverall3_2_2  = 0
 MaxUsedSliceOverall3_2_3: 5 x(1,2,4,6) + 4 x(1,2,4,5) + 3 x(1,2,4,4)
                           + 5 x(2,2,4,6) + 4 x(2,2,4,5) + 3 x(2,2,4,4)
                           - 2 maxSliceOverall - RgMaxUsedSliceOverall3_2_3
                            = 0
 MaxUsedSliceOverall3_2_4: 6 x(1,2,4,7) + 5 x(1,2,4,6) + 4 x(1,2,4,5)
                           + 3 x(1,2,4,4) + 6 x(2,2,4,7) + 5 x(2,2,4,6)
                           + 4 x(2,2,4,5) + 3 x(2,2,4,4) - 2 maxSliceOverall
                           - RgMaxUsedSliceOverall3_2_4  = 0
 MaxUsedSliceOverall3_2_5: 6 x(1,2,4,7) + 5 x(1,2,4,6) + 4 x(1,2,4,5)
                           + 6 x(2,2,4,7) + 5 x(2,2,4,6) + 4 x(2,2,4,5)
                           - 2 maxSliceOverall - RgMaxUsedSliceOverall3_2_5
                            = 0
 MaxUsedSliceOverall3_2_6: 6 x(1,2,4,7) + 5 x(1,2,4,6) + 6 x(2,2,4,7)
                           + 5 x(2,2,4,6) - 2 maxSliceOverall
                           - RgMaxUsedSliceOverall3_2_6  = 0
 MaxUsedSliceOverall3_2_7: 6 x(1,2,4,7) + 6 x(2,2,4,7) - 2 maxSliceOverall
                           - RgMaxUsedSliceOverall3_2_7  = 0
 MaxUsedSliceOverall3_3_1: 3 x(1,3,4,4) + 3 x(2,3,4,4) - 2 maxSliceOverall
                           - RgMaxUsedSliceOverall3_3_1  = 0
 MaxUsedSliceOverall3_3_2: 4 x(1,3,4,5) + 3 x(1,3,4,4) + 4 x(2,3,4,5)
                           + 3 x(2,3,4,4) - 2 maxSliceOverall
                           - RgMaxUsedSliceOverall3_3_2  = 0
 MaxUsedSliceOverall3_3_3: 5 x(1,3,4,6) + 4 x(1,3,4,5) + 3 x(1,3,4,4)
                           + 5 x(2,3,4,6) + 4 x(2,3,4,5) + 3 x(2,3,4,4)
                           - 2 maxSliceOverall - RgMaxUsedSliceOverall3_3_3
                            = 0
 MaxUsedSliceOverall3_3_4: 6 x(1,3,4,7) + 5 x(1,3,4,6) + 4 x(1,3,4,5)
                           + 3 x(1,3,4,4) + 6 x(2,3,4,7) + 5 x(2,3,4,6)
                           + 4 x(2,3,4,5) + 3 x(2,3,4,4) - 2 maxSliceOverall
                           - RgMaxUsedSliceOverall3_3_4  = 0
 MaxUsedSliceOverall3_3_5: 6 x(1,3,4,7) + 5 x(1,3,4,6) + 4 x(1,3,4,5)
                           + 6 x(2,3,4,7) + 5 x(2,3,4,6) + 4 x(2,3,4,5)
                           - 2 maxSliceOverall - RgMaxUsedSliceOverall3_3_5
                            = 0
 MaxUsedSliceOverall3_3_6: 6 x(1,3,4,7) + 5 x(1,3,4,6) + 6 x(2,3,4,7)
                           + 5 x(2,3,4,6) - 2 maxSliceOverall
                           - RgMaxUsedSliceOverall3_3_6  = 0
 MaxUsedSliceOverall3_3_7: 6 x(1,3,4,7) + 6 x(2,3,4,7) - 2 maxSliceOverall
                           - RgMaxUsedSliceOverall3_3_7  = 0
 MaxUsedSliceOverall3_4_1: - 3 maxSliceOverall - RgMaxUsedSliceOverall3_4_1
                            = 0
 MaxUsedSliceOverall3_4_2: - 3 maxSliceOverall - RgMaxUsedSliceOverall3_4_2
                            = 0
 MaxUsedSliceOverall3_4_3: - 3 maxSliceOverall - RgMaxUsedSliceOverall3_4_3
                            = 0
 MaxUsedSliceOverall3_4_4: - 3 maxSliceOverall - RgMaxUsedSliceOverall3_4_4
                            = 0
 MaxUsedSliceOverall3_4_5: - 3 maxSliceOverall - RgMaxUsedSliceOverall3_4_5
                            = 0
 MaxUsedSliceOverall3_4_6: - 3 maxSliceOverall - RgMaxUsedSliceOverall3_4_6
                            = 0
 MaxUsedSliceOverall3_4_7: - 3 maxSliceOverall - RgMaxUsedSliceOverall3_4_7
                            = 0
Bounds
 0 <= x(1,3,4,7) <= 1
 0 <= x(1,3,4,6) <= 1
 0 <= x(1,3,4,5) <= 1
 0 <= x(1,3,4,4) <= 1
 0 <= x(1,2,4,7) <= 1
 0 <= x(1,1,2,4) <= 1
 0 <= x(1,1,3,4) <= 1
 0 <= x(1,1,4,4) <= 1
 0 <= x(1,1,2,5) <= 1
 0 <= x(1,1,3,5) <= 1
 0 <= x(1,1,4,5) <= 1
 0 <= x(1,1,2,6) <= 1
 0 <= x(1,1,3,6) <= 1
 0 <= x(1,1,4,6) <= 1
 0 <= x(1,1,4,7) <= 1
 0 <= x(1,1,3,7) <= 1
 0 <= x(1,1,2,7) <= 1
 0 <= x(1,2,4,6) <= 1
 0 <= x(1,2,4,5) <= 1
 0 <= x(1,2,4,4) <= 1
 0 <= x(2,3,4,7) <= 1
 0 <= x(2,3,4,6) <= 1
 0 <= x(2,3,4,5) <= 1
 0 <= x(2,3,4,4) <= 1
 0 <= x(2,2,4,7) <= 1
 0 <= x(2,1,2,4) <= 1
 0 <= x(2,1,3,4) <= 1
 0 <= x(2,1,4,4) <= 1
 0 <= x(2,1,2,5) <= 1
 0 <= x(2,1,3,5) <= 1
 0 <= x(2,1,4,5) <= 1
 0 <= x(2,1,2,6) <= 1
 0 <= x(2,1,3,6) <= 1
 0 <= x(2,1,4,6) <= 1
 0 <= x(2,1,4,7) <= 1
 0 <= x(2,1,3,7) <= 1
 0 <= x(2,1,2,7) <= 1
 0 <= x(2,2,4,6) <= 1
 0 <= x(2,2,4,5) <= 1
 0 <= x(2,2,4,4) <= 1
 0 <= maxSlice(1) <= 7
 0 <= maxSlice(2) <= 7
 0 <= maxSlice(3) <= 7
 0 <= maxSlice(4) <= 7
 0 <= maxSlice(5) <= 7
 0 <= maxSliceOverall <= 6
 0 <= RgSource_2_1 <= 1
 0 <= RgSource_3_1 <= 1
 0 <= RgSource_2_2 <= 1
 0 <= RgSource_3_2 <= 1
 0 <= RgOSNR_1 <= 500
 0 <= RgOSNR_2 <= 500
 0 <= RgLength_1 <= 500
 0 <= RgLength_2 <= 500
 0 <= RgNonOverlap_1_1 <= 1
 0 <= RgNonOverlap_1_2 <= 1
 0 <= RgNonOverlap_1_3 <= 1
 0 <= RgNonOverlap_1_4 <= 1
 0 <= RgNonOverlap_1_5 <= 1
 0 <= RgNonOverlap_1_6 <= 1
 0 <= RgNonOverlap_1_7 <= 1
 0 <= RgNonOverlap_2_1 <= 1
 0 <= RgNonOverlap_2_2 <= 1
 0 <= RgNonOverlap_2_3 <= 1
 0 <= RgNonOverlap_2_4 <= 1
 0 <= RgNonOverlap_2_5 <= 1
 0 <= RgNonOverlap_2_6 <= 1
 0 <= RgNonOverlap_2_7 <= 1
 0 <= RgNonOverlap_3_1 <= 1
 0 <= RgNonOverlap_3_2 <= 1
 0 <= RgNonOverlap_3_3 <= 1
 0 <= RgNonOverlap_3_4 <= 1
 0 <= RgNonOverlap_3_5 <= 1
 0 <= RgNonOverlap_3_6 <= 1
 0 <= RgNonOverlap_3_7 <= 1
 0 <= RgNonOverlap_4_1 <= 1
 0 <= RgNonOverlap_4_2 <= 1
 0 <= RgNonOverlap_4_3 <= 1
 0 <= RgNonOverlap_4_4 <= 1
 0 <= RgNonOverlap_4_5 <= 1
 0 <= RgNonOverlap_4_6 <= 1
 0 <= RgNonOverlap_4_7 <= 1
 0 <= RgNonOverlap_5_1 <= 1
 0 <= RgNonOverlap_5_2 <= 1
 0 <= RgNonOverlap_5_3 <= 1
 0 <= RgNonOverlap_5_4 <= 1
 0 <= RgNonOverlap_5_5 <= 1
 0 <= RgNonOverlap_5_6 <= 1
 0 <= RgNonOverlap_5_7 <= 1
-7 <= RgMaxUsedSlicePerLink_1_1 <= 0
-7 <= RgMaxUsedSlicePerLink_1_2 <= 0
-7 <= RgMaxUsedSlicePerLink_2_1 <= 0
-7 <= RgMaxUsedSlicePerLink_2_2 <= 0
-7 <= RgMaxUsedSlicePerLink_3_1 <= 0
-7 <= RgMaxUsedSlicePerLink_3_2 <= 0
-7 <= RgMaxUsedSlicePerLink_4_1 <= 0
-7 <= RgMaxUsedSlicePerLink_4_2 <= 0
-7 <= RgMaxUsedSlicePerLink_5_1 <= 0
-7 <= RgMaxUsedSlicePerLink_5_2 <= 0
-6 <= RgMaxUsedSliceOverall_1 <= 0
-6 <= RgMaxUsedSliceOverall_2 <= 0
-6 <= RgMaxUsedSliceOverall2_1_1 <= 0
-6 <= RgMaxUsedSliceOverall2_1_2 <= 0
-6 <= RgMaxUsedSliceOverall2_1_3 <= 0
-6 <= RgMaxUsedSliceOverall2_1_4 <= 0
-6 <= RgMaxUsedSliceOverall2_1_5 <= 0
-6 <= RgMaxUsedSliceOverall2_1_6 <= 0
-6 <= RgMaxUsedSliceOverall2_1_7 <= 0
-6 <= RgMaxUsedSliceOverall2_2_1 <= 0
-6 <= RgMaxUsedSliceOverall2_2_2 <= 0
-6 <= RgMaxUsedSliceOverall2_2_3 <= 0
-6 <= RgMaxUsedSliceOverall2_2_4 <= 0
-6 <= RgMaxUsedSliceOverall2_2_5 <= 0
-6 <= RgMaxUsedSliceOverall2_2_6 <= 0
-6 <= RgMaxUsedSliceOverall2_2_7 <= 0
-6 <= RgMaxUsedSliceOverall2_3_1 <= 0
-6 <= RgMaxUsedSliceOverall2_3_2 <= 0
-6 <= RgMaxUsedSliceOverall2_3_3 <= 0
-6 <= RgMaxUsedSliceOverall2_3_4 <= 0
-6 <= RgMaxUsedSliceOverall2_3_5 <= 0
-6 <= RgMaxUsedSliceOverall2_3_6 <= 0
-6 <= RgMaxUsedSliceOverall2_3_7 <= 0
-6 <= RgMaxUsedSliceOverall2_4_1 <= 0
-6 <= RgMaxUsedSliceOverall2_4_2 <= 0
-6 <= RgMaxUsedSliceOverall2_4_3 <= 0
-6 <= RgMaxUsedSliceOverall2_4_4 <= 0
-6 <= RgMaxUsedSliceOverall2_4_5 <= 0
-6 <= RgMaxUsedSliceOverall2_4_6 <= 0
-6 <= RgMaxUsedSliceOverall2_4_7 <= 0
-6 <= RgMaxUsedSliceOverall2_5_1 <= 0
-6 <= RgMaxUsedSliceOverall2_5_2 <= 0
-6 <= RgMaxUsedSliceOverall2_5_3 <= 0
-6 <= RgMaxUsedSliceOverall2_5_4 <= 0
-6 <= RgMaxUsedSliceOverall2_5_5 <= 0
-6 <= RgMaxUsedSliceOverall2_5_6 <= 0
-6 <= RgMaxUsedSliceOverall2_5_7 <= 0
-18 <= RgMaxUsedSliceOverall3_1_1 <= 0
-18 <= RgMaxUsedSliceOverall3_1_2 <= 0
-18 <= RgMaxUsedSliceOverall3_1_3 <= 0
-18 <= RgMaxUsedSliceOverall3_1_4 <= 0
-18 <= RgMaxUsedSliceOverall3_1_5 <= 0
-18 <= RgMaxUsedSliceOverall3_1_6 <= 0
-18 <= RgMaxUsedSliceOverall3_1_7 <= 0
-12 <= RgMaxUsedSliceOverall3_2_1 <= 0
-12 <= RgMaxUsedSliceOverall3_2_2 <= 0
-12 <= RgMaxUsedSliceOverall3_2_3 <= 0
-12 <= RgMaxUsedSliceOverall3_2_4 <= 0
-12 <= RgMaxUsedSliceOverall3_2_5 <= 0
-12 <= RgMaxUsedSliceOverall3_2_6 <= 0
-12 <= RgMaxUsedSliceOverall3_2_7 <= 0
-12 <= RgMaxUsedSliceOverall3_3_1 <= 0
-12 <= RgMaxUsedSliceOverall3_3_2 <= 0
-12 <= RgMaxUsedSliceOverall3_3_3 <= 0
-12 <= RgMaxUsedSliceOverall3_3_4 <= 0
-12 <= RgMaxUsedSliceOverall3_3_5 <= 0
-12 <= RgMaxUsedSliceOverall3_3_6 <= 0
-12 <= RgMaxUsedSliceOverall3_3_7 <= 0
-18 <= RgMaxUsedSliceOverall3_4_1 <= 0
-18 <= RgMaxUsedSliceOverall3_4_2 <= 0
-18 <= RgMaxUsedSliceOverall3_4_3 <= 0
-18 <= RgMaxUsedSliceOverall3_4_4 <= 0
-18 <= RgMaxUsedSliceOverall3_4_5 <= 0
-18 <= RgMaxUsedSliceOverall3_4_6 <= 0
-18 <= RgMaxUsedSliceOverall3_4_7 <= 0
Binaries
 x(1,3,4,7)  x(1,3,4,6)  x(1,3,4,5)  x(1,3,4,4)  x(1,2,4,7)  x(1,1,2,4) 
 x(1,1,3,4)  x(1,1,4,4)  x(1,1,2,5)  x(1,1,3,5)  x(1,1,4,5)  x(1,1,2,6) 
 x(1,1,3,6)  x(1,1,4,6)  x(1,1,4,7)  x(1,1,3,7)  x(1,1,2,7)  x(1,2,4,6) 
 x(1,2,4,5)  x(1,2,4,4)  x(2,3,4,7)  x(2,3,4,6)  x(2,3,4,5)  x(2,3,4,4) 
 x(2,2,4,7)  x(2,1,2,4)  x(2,1,3,4)  x(2,1,4,4)  x(2,1,2,5)  x(2,1,3,5) 
 x(2,1,4,5)  x(2,1,2,6)  x(2,1,3,6)  x(2,1,4,6)  x(2,1,4,7)  x(2,1,3,7) 
 x(2,1,2,7)  x(2,2,4,6)  x(2,2,4,5)  x(2,2,4,4) 
End
