#include "manipTools.h"

template<class OBJS>
void addBoxPickObjectives(OBJS& komo, double time, rai::ArgWord dir, const char* boxName, const arr& boxSize, const char* gripperName, const char* palmName, const char* tableName) {
    arr xLine, yzPlane;
    FeatureSymbol xyScalarProduct=FS_none, xzScalarProduct=FS_none;
    if(dir==rai::_xAxis){
        xLine = {{1,3},{1,0,0}};
        yzPlane = {{2,3},{0,1,0,0,0,1}};
        xyScalarProduct = FS_scalarProductXY;
        xzScalarProduct = FS_scalarProductXZ;
    } else if(dir==rai::_yAxis){
        xLine = {{1,3},{0,1,0}};
        yzPlane = {{2,3},{1,0,0,0,0,1}};
        xyScalarProduct = FS_scalarProductXX;
        xzScalarProduct = FS_scalarProductXZ;
    } else if(dir==rai::_zAxis){
        xLine = {{1,3},{0,0,1}};
        yzPlane = {{2,3},{1,0,0,0,1,0}};
        xyScalarProduct = FS_scalarProductXX;
        xzScalarProduct = FS_scalarProductXY;
    }

    double margin=.02;

    //position: center in inner target plane; X-specific
    komo.addObjective({time}, FS_positionRel, {gripperName, boxName}, OT_eq, xLine*1e2, {});
    komo.addObjective({time}, FS_positionRel, {gripperName, boxName}, OT_ineq, yzPlane*1e2, (boxSize/2.-margin));
    komo.addObjective({time}, FS_positionRel, {gripperName, boxName}, OT_ineq, yzPlane*(-1e2), -(boxSize/2.-margin));

    //orientation: grasp axis orthoginal to target plane; X-specific
    komo.addObjective({time-.2,time}, xyScalarProduct, {gripperName, boxName}, OT_eq, {1e2}, {});
    komo.addObjective({time-.2,time}, xzScalarProduct, {gripperName, boxName}, OT_eq, {1e2}, {});

    //no collision with palm
    komo.addObjective({time-.3,time}, FS_distance, {palmName, boxName}, OT_ineq, {1e1}, {-.001});
    //  komo.addObjective({time-.5,time}, FS_distance, {palmName, tableName}, OT_ineq, {1e1}, {-.05});

    //approach: only longitudial velocity, min distance before and at grasp
    if(komo.k_order>1) komo.addObjective({time-.3,time}, FS_positionRel, {boxName, gripperName}, OT_eq, arr{{2,3}, {1,0,0,0,1,0}}*1e2, {}, 1);
    if(komo.k_order>1) komo.addObjective({time-.5,time-.3}, FS_distance, {palmName, boxName}, OT_ineq, {1e1}, {-.1});

    //zero vel
    if(komo.k_order>1) komo.addObjective({time}, FS_qItself, {}, OT_eq, {}, {}, 1);
}

template<class OBJS>
void addBoxPlaceObjectives(OBJS& komo, double time,
                           rai::ArgWord dir, const char* boxName, const arr& boxSize,
                           const char* tableName,
                           const char* gripperName, const char* palmName) {
    double relPos=0.;
    FeatureSymbol zVector = FS_none;
    arr zVectorTarget = {0.,0.,1.};
    if(dir==rai::_xAxis){
        relPos = .5*boxSize(0)+.05;
        zVector = FS_vectorX;
    } else if(dir==rai::_yAxis){
        relPos = .5*boxSize(1)+.05;
        zVector = FS_vectorY;
    } else if(dir==rai::_zAxis){
        relPos = .5*boxSize(2)+.05;
        zVector = FS_vectorZ;
    } else if(dir==rai::_xNegAxis){
        relPos = .5*boxSize(0)+.05;
        zVector = FS_vectorX;
        zVectorTarget *= -1.;
    } else if(dir==rai::_yNegAxis){
        relPos = .5*boxSize(1)+.05;
        zVector = FS_vectorY;
        zVectorTarget *= -1.;
    } else if(dir==rai::_zNegAxis){
        relPos = .5*boxSize(2)+.05;
        zVector = FS_vectorZ;
        zVectorTarget *= -1.;
    }

    //z-position: fixed
    komo.addObjective({time}, FS_positionDiff, {boxName, tableName}, OT_eq, 1e2*arr({1,3},{0,0,1}), {.0, .0, relPos});

    //xy-position: above table
    komo.addObjective({time}, FS_aboveBox, {boxName, tableName}, OT_ineq, {1e2});

    //orientation: Y-up
    komo.addObjective({time-.2, time}, zVector, {boxName}, OT_eq, {1e1}, zVectorTarget);

    //retract: only longitudial velocity, min distance after grasp
    if(komo.k_order>1) komo.addObjective({time,time+.3}, FS_positionRel, {boxName, gripperName}, OT_eq, arr{{2,3}, {1,0,0,0,1,0}}*1e2, {}, 1);
    if(komo.k_order>1) komo.addObjective({time+.3,time+.5}, FS_distance, {palmName, boxName}, OT_ineq, {1e1}, {-.1});

    //zero vel
    if(komo.k_order>1) komo.addObjective({time}, FS_qItself, {}, OT_eq, {}, {}, 1);
}

//INSTANTIATE
template
void addBoxPickObjectives(KOMO& komo, double time, rai::ArgWord dir,
                          const char* boxName, const arr& boxSize,
                          const char* gripperName, const char* palmName, const char* tableName);

template
void addBoxPlaceObjectives(KOMO& komo, double time,
                           rai::ArgWord dir, const char* boxName, const arr& boxSize, const char* tableName,
                           const char* gripperName, const char* palmName);
