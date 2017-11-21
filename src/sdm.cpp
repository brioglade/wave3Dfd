#include "sdm.hpp"

SDM::SDM(VecF IGI, VecF IGF,VecI I_NodG,VecF IlimI, VecF IlimF, VecI INod, \
		Dfloat If0, Dfloat Idt, VecI INsdm) {

	// GI INITIAL GLOBAL LIMIT WITH PML 
	// GF END GLOBAL LIMIT WITH PML
	// NodG NUMBER OF GLOBAL NODES WITHOUT PML
	// IlimI INITIAL LIMIT SUB DOMAIN 
	// IlimF END LIMIT SUB DOMAIN
	// INOD   NUMBER OF LOCAL NODES WITHOUT HALO SUBDOMAIN
	// f0 FREQUENCY
	// dt DELTA T
        // Nsdm SUBDOMAIN INDEX

	GI = IGI;
	GF = IGF;
	f0 = If0;
	dt = Idt;
	NodG = I_NodG;
	HALO.x = KHALO;
	HALO.y = KHALO;
	HALO.z = KHALO;
	NodLoc = INod;
	Nsdm = INsdm;

	VecI ELE={INod.x-1,INod.y-1,INod.z-1};

	SDMGeom = new geometry3D(IlimI,IlimF,ELE,HALO);

	thickness_PML = SDMGeom->thickness_PML();

	NT.x = NodG.x + 2 * PML.x;
	NT.y = NodG.y + 2 * PML.y;
	NT.z = NodG.z + PML.z;

	pml_x = new DPML(NodG.x,NT.x,thickness_PML.x,SDMGeom->Dx(),PML.x,dt,f0, \
		PML_XMIN,PML_XMAX);

	pml_y = new DPML(NodG.y,NT.y,thickness_PML.y,SDMGeom->Dy(),PML.y,dt,f0, \
		PML_YMIN,PML_YMAX);

	pml_z = new DPML(NodG.z,NT.z,thickness_PML.z,SDMGeom->Dz(),PML.z,dt,f0, \
		PML_ZMIN,PML_ZMAX);



	sxx = new Dfloat [SDMGeom->HALO_Node()];
	syy = new Dfloat [SDMGeom->HALO_Node()];
	szz = new Dfloat [SDMGeom->HALO_Node()];
	sxy = new Dfloat [SDMGeom->HALO_Node()];
	sxz = new Dfloat [SDMGeom->HALO_Node()];
	syz = new Dfloat [SDMGeom->HALO_Node()];
	vx = new Dfloat [SDMGeom->HALO_Node()];
	vy = new Dfloat [SDMGeom->HALO_Node()];
	vz = new Dfloat [SDMGeom->HALO_Node()];
	ux = new Dfloat [SDMGeom->HALO_Node()];
	uy = new Dfloat [SDMGeom->HALO_Node()];
	uz = new Dfloat [SDMGeom->HALO_Node()];
	mu = new Dfloat [SDMGeom->HALO_Node()];
	lamb = new Dfloat [SDMGeom->HALO_Node()];
	rho = new Dfloat [SDMGeom->HALO_Node()];
	dsxx_dx = new Dfloat [SDMGeom->HALO_Node()];
	dsxy_dy = new Dfloat [SDMGeom->HALO_Node()];
	dsxz_dz = new Dfloat [SDMGeom->HALO_Node()];
	dsxy_dx = new Dfloat [SDMGeom->HALO_Node()];
	dsyy_dy = new Dfloat [SDMGeom->HALO_Node()];
	dsyz_dz = new Dfloat [SDMGeom->HALO_Node()];
	dsxz_dx = new Dfloat [SDMGeom->HALO_Node()];
	dsyz_dy = new Dfloat [SDMGeom->HALO_Node()];
	dszz_dz = new Dfloat [SDMGeom->HALO_Node()];
	dvx_dx = new Dfloat [SDMGeom->HALO_Node()];
	dvy_dy = new Dfloat [SDMGeom->HALO_Node()];
	dvz_dz = new Dfloat [SDMGeom->HALO_Node()];
	dvx_dy = new Dfloat [SDMGeom->HALO_Node()];
	dvy_dx = new Dfloat [SDMGeom->HALO_Node()];
	dvx_dz = new Dfloat [SDMGeom->HALO_Node()];
	dvz_dx = new Dfloat [SDMGeom->HALO_Node()];
	dvy_dz = new Dfloat [SDMGeom->HALO_Node()];
	dvz_dy = new Dfloat [SDMGeom->HALO_Node()];


}


SDM::~SDM(){

	delete [] pml_x;
	delete [] pml_y;
	delete [] pml_z;
	delete [] SDMGeom;
	delete [] sxx;
	delete [] syy;
	delete [] szz;
	delete [] sxy;
	delete [] sxz;
	delete [] syz;
	delete [] vx;
	delete [] vy;
	delete [] vz;
	delete [] ux;
	delete [] uy;
	delete [] uz;
	delete [] mu;
	delete [] lamb;
	delete [] rho;
	delete [] dsxx_dx;
	delete [] dsxy_dy;
	delete [] dsxz_dz;
	delete [] dsxy_dx;
	delete [] dsyy_dy;
	delete [] dsyz_dz;
	delete [] dsxz_dx;
	delete [] dsyz_dy;
	delete [] dszz_dz;
	delete [] dvx_dx;
	delete [] dvy_dy;
	delete [] dvz_dz;
	delete [] dvx_dy;
	delete [] dvy_dx;
	delete [] dvx_dz;
	delete [] dvz_dx;
	delete [] dvy_dz;
	delete [] dvz_dy;

}


// INDEX_IJK

int SDM::IJK(int i, int j, int k){

  int indx = i + j * SDMGeom->HALO_NodeX() + k * SDMGeom->HALO_NodeX() * \
    SDMGeom->HALO_NodeY();

  return indx;

}


void SDM::ModelRead(Dfloat *model,char *param){

  if (strcmp("RHO",param) == 0){

    for (int i=0; i<SDMGeom->HALO_Node(); ++i)
      rho[i] = model[i];

  }

  
  if (strcmp("MU",param) == 0){

    for (int i=0; i<SDMGeom->HALO_Node(); ++i)
      mu[i] = model[i];

  }

  
  if (strcmp("LAMB",param) == 0){

    for (int i=0; i<SDMGeom->HALO_Node(); ++i)
      lamb[i] = model[i];

  }

}


int SDM::CFL(){

	for (int i=0; i<SDMGeom->HALO_Node(); ++i){

		if ((mu[i] < 0.0) || (rho[i] < 0.0) || (lamb[i] < 0.0))
		{
			std::cout<<"ERROR IN MODEL PARAMETERS"<<std::endl;

		} else {


		       if ((mu[i] == 0.0) || (rho[i] == 0.0) || (lamb[i] == 0.0))
			   break;

			Dfloat vp;

			vp = sqrt((lamb[i] + 2.0 * mu[i]) / rho[i]);

			VecF K;

			K.x = SDMGeom->Dx() / ( dt * sqrt(2.0 * vp * (C0 + C1)));
			K.y = SDMGeom->Dy() / ( dt * sqrt(2.0 * vp * (C0 + C1)));
			K.z = SDMGeom->Dz() / ( dt * sqrt(2.0 * vp * (C0 + C1)));

			if ((K.x >= 1.0) || (K.y >= 1.0) || (K.z >= 1.0))
			{
				std::cout<<"CFL NOT SATISFIED"<<std::endl;
				return 0;	
			} else{

				return 1;
			}

		}


	}


}



void SDM::InitVar(Dfloat f){


  for (int i=0; i<SDMGeom->HALO_Node(); ++i){


    sxx[i] = f;
    syy[i] = f;
    szz[i] = f;
    sxy[i] = f;
    sxz[i] = f;
    syz[i] = f;
    vx[i] = f;
    vy[i] = f;
    vz[i] = f;
    ux[i] = f;
    uy[i] = f;
    uz[i] = f;
    dsxx_dx[i] = f;
    dsxy_dy[i] = f;
    dsxz_dz[i] = f;
    dsxy_dx[i] = f;
    dsyy_dy[i] = f;
    dsyz_dz[i] = f;
    dsxz_dx[i] = f;
    dsyz_dy[i] = f;
    dszz_dz[i] = f;
    dvx_dx[i] = f;
    dvy_dy[i] = f;
    dvz_dz[i] = f;
    dvx_dy[i] = f;
    dvy_dx[i] = f;
    dvx_dz[i] = f;
    dvz_dx[i] = f;
    dvy_dz[i] = f;
    dvz_dy[i] = f;

  }

  

}


void SDM::EB(Dfloat *BN,Dfloat* DomLoc, char *param){
  int indx1,indx2;
  
  if (strcmp("S",param) == 0){

    for (int k=0;k<SDMGeom->L_NodeZ();k++){
      for (int j=0;j<HALO.y;j++){
	for (int i=0;i<SDMGeom->L_NodeX();i++){

	  indx1 = i + j * NodLoc.x + k * HALO.y * NodLoc.x;
	  indx2 = (HALO.x + i) + (j + HALO.y) * SDMGeom->HALO_NodeX() \
	    + (k + HALO.z) * SDMGeom->HALO_NodeX() * SDMGeom->HALO_NodeY() ;	  

	  BN[indx1] = DomLoc[indx2];
	  
	}
      }
    }


  }else if (strcmp("N",param) == 0){

    for (int k=0;k<SDMGeom->L_NodeZ();k++){
      for (int j=0;j<HALO.y;j++){
	for (int i=0;i<SDMGeom->L_NodeX();i++){

	  indx1 = i + j * NodLoc.x + k * HALO.y * NodLoc.x;
	  indx2 = (HALO.x + i) + (j + SDMGeom->HALO_NodeY() - 2) * SDMGeom->HALO_NodeX() \
	    + (k + HALO.z) * SDMGeom->HALO_NodeX() * SDMGeom->HALO_NodeY() ;	  

	  BN[indx1] = DomLoc[indx2];
	  
	}
      }
    }


  }else if (strcmp("O",param) == 0){


    for (int k=0;k<SDMGeom->L_NodeZ();k++){
      for (int j=0;j<SDMGeom->L_NodeY();j++){
	for (int i=0;i<HALO.x;i++){

	  indx1 = i + j * HALO.x + k * NodLoc.y * HALO.x;
	  indx2 = (HALO.x + i) + (j + HALO.y) * SDMGeom->HALO_NodeX() \
	    + (k + HALO.z) * SDMGeom->HALO_NodeX() * SDMGeom->HALO_NodeY() ;	  

	  BN[indx1] = DomLoc[indx2];
	  
	}
      }
    }




  }else if (strcmp("E",param) == 0){


    for (int k=0;k<SDMGeom->L_NodeZ();k++){
      for (int j=0;j<SDMGeom->L_NodeY();j++){
	for (int i=0;i<HALO.x;i++){

	  indx1 = i + j * HALO.x + k * NodLoc.y * HALO.x;
	  indx2 = (SDMGeom->HALO_NodeX() + i - 2) + (j + HALO.y) * SDMGeom->HALO_NodeX() \
	    + (k + HALO.z) * SDMGeom->HALO_NodeX() * SDMGeom->HALO_NodeY() ;	  

	  BN[indx1] = DomLoc[indx2];
	  
	}
      }
    }



  }else if (strcmp("UP",param) == 0){

    for (int k=0;k<HALO.z;k++){
      for (int j=0;j<SDMGeom->L_NodeY();j++){
	for (int i=0;i<SDMGeom->L_NodeX();i++){

	  indx1 = i + j * NodLoc.x + k * NodLoc.y * NodLoc.x;
	  indx2 = (HALO.x + i) + (j + HALO.y) * SDMGeom->HALO_NodeX() \
	    + (k + SDMGeom->HALO_NodeZ() - 2 ) * SDMGeom->HALO_NodeX() * SDMGeom->HALO_NodeY();	  

	  BN[indx1] = DomLoc[indx2];
	  
	}
      }
    }



  }else if (strcmp("DOWN",param) == 0){



    for (int k=0;k<HALO.z;k++){
      for (int j=0;j<SDMGeom->L_NodeY();j++){
	for (int i=0;i<SDMGeom->L_NodeX();i++){

	  indx1 = i + j * NodLoc.x + k * NodLoc.y * NodLoc.x;
	  indx2 = (HALO.x + i) + (j + HALO.y) * SDMGeom->HALO_NodeX() \
	    + (k + HALO.z ) * SDMGeom->HALO_NodeX() * SDMGeom->HALO_NodeY();	  

	  BN[indx1] = DomLoc[indx2];
	  
	}
      }
    }



  } else {

    std::cout<<"THAT IS NOT AN BOUNDARY OPTION"<<std::endl;


  }


}


void SDM::IB(Dfloat *BN, Dfloat *DomLoc, char *param) {
  int indx1,indx2;

  if (strcmp("S",param) == 0){

    for (int k=0;k<SDMGeom->L_NodeZ();k++){
      for (int j=0;j<HALO.y;j++){
	for (int i=0;i<SDMGeom->L_NodeX();i++){

	  indx1 = i + j * NodLoc.x + k * HALO.y * NodLoc.x;
	  indx2 = (HALO.x + i) + (j + HALO.y) * SDMGeom->HALO_NodeX() \
	    + (k + HALO.z) * SDMGeom->HALO_NodeX() * SDMGeom->HALO_NodeY() ;	  

	  DomLoc[indx2] = BN[indx1];
	  
	}
      }
    }


  }else if (strcmp("N",param) == 0){

    for (int k=0;k<SDMGeom->L_NodeZ();k++){
      for (int j=0;j<HALO.y;j++){
	for (int i=0;i<SDMGeom->L_NodeX();i++){

	  indx1 = i + j * NodLoc.x + k * HALO.y * NodLoc.x;
	  indx2 = (HALO.x + i) + (j + SDMGeom->HALO_NodeY() - 2) * SDMGeom->HALO_NodeX() \
	    + (k + HALO.z) * SDMGeom->HALO_NodeX() * SDMGeom->HALO_NodeY() ;	  

	  DomLoc[indx2] = BN[indx1];
	  
	}
      }
    }


  }else if (strcmp("O",param) == 0){


    for (int k=0;k<SDMGeom->L_NodeZ();k++){
      for (int j=0;j<SDMGeom->L_NodeY();j++){
	for (int i=0;i<HALO.x;i++){

	  indx1 = i + j * HALO.x + k * NodLoc.y * HALO.x;
	  indx2 = (HALO.x + i) + (j + HALO.y) * SDMGeom->HALO_NodeX() \
	    + (k + HALO.z) * SDMGeom->HALO_NodeX() * SDMGeom->HALO_NodeY() ;	  

	  DomLoc[indx2] = BN[indx1];
	  
	}
      }
    }




  }else if (strcmp("E",param) == 0){


    for (int k=0;k<SDMGeom->L_NodeZ();k++){
      for (int j=0;j<SDMGeom->L_NodeY();j++){
	for (int i=0;i<HALO.x;i++){

	  indx1 = i + j * HALO.x + k * NodLoc.y * HALO.x;
	  indx2 = (SDMGeom->HALO_NodeX() + i - 2) + (j + HALO.y) * SDMGeom->HALO_NodeX() \
	    + (k + HALO.z) * SDMGeom->HALO_NodeX() * SDMGeom->HALO_NodeY() ;	  

	  DomLoc[indx2] = BN[indx1];
	  
	}
      }
    }



  }else if (strcmp("UP",param) == 0){

    for (int k=0;k<HALO.z;k++){
      for (int j=0;j<SDMGeom->L_NodeY();j++){
	for (int i=0;i<SDMGeom->L_NodeX();i++){

	  indx1 = i + j * NodLoc.x + k * NodLoc.y * NodLoc.x;
	  indx2 = (HALO.x + i) + (j + HALO.y) * SDMGeom->HALO_NodeX() \
	    + (k + SDMGeom->HALO_NodeZ() - 2 ) * SDMGeom->HALO_NodeX() * SDMGeom->HALO_NodeY();	  

	  DomLoc[indx2] = BN[indx1];
	  
	}
      }
    }



  }else if (strcmp("DOWN",param) == 0){



    for (int k=0;k<HALO.z;k++){
      for (int j=0;j<SDMGeom->L_NodeY();j++){
	for (int i=0;i<SDMGeom->L_NodeX();i++){

	  indx1 = i + j * NodLoc.x + k * NodLoc.y * NodLoc.x;
	  indx2 = (HALO.x + i) + (j + HALO.y) * SDMGeom->HALO_NodeX() \
	    + (k + HALO.z ) * SDMGeom->HALO_NodeX() * SDMGeom->HALO_NodeY();	  

	  DomLoc[indx2] = BN[indx1];
	  
	}
      }
    }



  } else {

    std::cout<<"THAT IS NOT AN BOUNDARY OPTION"<<std::endl;


  }


}



void SDM::ExpBoundary(Dfloat *BN, char *TBound,char *NameVar) {

  if (strcmp("SXX",NameVar) == 0){
    EB(BN,sxx,TBound);
  }
  if (strcmp("SYY",NameVar) == 0){
    EB(BN,syy,TBound);
  }

  if (strcmp("SZZ",NameVar) == 0){
    EB(BN,szz,TBound);
  }

  if (strcmp("SXY",NameVar) == 0){
    EB(BN,sxy,TBound);
  }

  if (strcmp("SXZ",NameVar) == 0){
    EB(BN,sxz,TBound);
  }

  if (strcmp("SYZ",NameVar) == 0){
    EB(BN,syz,TBound);
  }

  if (strcmp("VX",NameVar) == 0){
    EB(BN,vx,TBound);
  }

  if (strcmp("VY",NameVar) == 0){
    EB(BN,vy,TBound);
  }

  if (strcmp("VZ",NameVar) == 0){
    EB(BN,vz,TBound);
  }
  
}


void SDM::ImpBoundary(Dfloat *BN, char *TBound,char *NameVar) {

  if (strcmp("SXX",NameVar) == 0){
    IB(BN,sxx,TBound);
  }
  if (strcmp("SYY",NameVar) == 0){
    IB(BN,syy,TBound);
  }

  if (strcmp("SZZ",NameVar) == 0){
    IB(BN,szz,TBound);
  }

  if (strcmp("SXY",NameVar) == 0){
    IB(BN,sxy,TBound);
  }

  if (strcmp("SXZ",NameVar) == 0){
    IB(BN,sxz,TBound);
  }

  if (strcmp("SYZ",NameVar) == 0){
    IB(BN,syz,TBound);
  }

  if (strcmp("VX",NameVar) == 0){
    IB(BN,vx,TBound);
  }

  if (strcmp("VY",NameVar) == 0){
    IB(BN,vy,TBound);
  }

  if (strcmp("VZ",NameVar) == 0){
    IB(BN,vz,TBound);
  }
  
}

Dfloat SDM::SCoorX(int indx){
  Dfloat coor = SDMGeom->CoorX(indx) - SDMGeom->thickness_PML().x;
  return coor;
}

Dfloat SDM::SCoorY(int indx){
  Dfloat coor = SDMGeom->CoorY(indx) - SDMGeom->thickness_PML().y;
  return coor;
}

Dfloat SDM::SCoorZ(int indx){
  Dfloat coor = SDMGeom->CoorZ(indx) - SDMGeom->thickness_PML().z;
  return coor;
}

Dfloat SDM::SCoorXHalf(int indx){
  Dfloat coor = SDMGeom->CoorXHalf(indx) - SDMGeom->thickness_PML().x;
  return coor;
}

Dfloat SDM::SCoorYHalf(int indx){
  Dfloat coor = SDMGeom->CoorYHalf(indx) - SDMGeom->thickness_PML().y;
  return coor;
}

Dfloat SDM::SCoorZHalf(int indx){
  Dfloat coor = SDMGeom->CoorZHalf(indx) - SDMGeom->thickness_PML().z;
  return coor;
}


VecI SDM::Loc2Glo(VecI indx){
  VecI ind;
  
  ind.x = indx.x - HALO.x + Nsdm.x * NodLoc.x;
  ind.y = indx.y - HALO.y + Nsdm.y * NodLoc.y;
  ind.z = indx.z - HALO.z + Nsdm.z * NodLoc.z;
  
  return ind;

}


VecI SDM::SFindNode(VecI Nodef){

  VecI ind;

  VecI Global,Local;
  
  for (int iz=0;iz<SDMGeom->HALO_NodeZ();iz++){
     for (int iy=0;iy<SDMGeom->HALO_NodeY();iy++){
       for (int ix=0;ix<SDMGeom->HALO_NodeX();ix++){

	 Local = {ix,iy,iz};
	 Global = Loc2Glo(Local);
   
	 if ( ((Nodef.x - Global.x) == 0) &&	\
	      ((Nodef.y - Global.y) == 0) &&	\
	      ((Nodef.z - Global.z) == 0) ) {
	  
	   ind.x = ix; 
	   ind.y = iy;
	   ind.z = iz;

	   goto FIN;
	 }
		       	 
       }
     }
   }

 FIN: return ind;
  
}

void SDM::AddVal(VecI indx, char *NameVar, Dfloat Val){
  VecI ind;
  int i;
  
   if (strcmp("SXX",NameVar) == 0){
    ind = SFindNode(indx);
    i = ind.x + ind.y * SDMGeom->HALO_NodeX() + ind.z * SDMGeom->HALO_NodeX() * \
      SDMGeom->HALO_NodeY();
    sxx[i] = Val;
  }
  if (strcmp("SYY",NameVar) == 0){
    ind = SFindNode(indx);
    i = ind.x + ind.y * SDMGeom->HALO_NodeX() + ind.z * SDMGeom->HALO_NodeX() * \
      SDMGeom->HALO_NodeY();
    syy[i] = Val;
  }

  if (strcmp("SZZ",NameVar) == 0){
    ind = SFindNode(indx);
    i = ind.x + ind.y * SDMGeom->HALO_NodeX() + ind.z * SDMGeom->HALO_NodeX() * \
      SDMGeom->HALO_NodeY();
    szz[i] = Val;
  }

  if (strcmp("SXY",NameVar) == 0){
    ind = SFindNode(indx);
    i = ind.x + ind.y * SDMGeom->HALO_NodeX() + ind.z * SDMGeom->HALO_NodeX() * \
      SDMGeom->HALO_NodeY();
    sxy[i] = Val;
  }

  if (strcmp("SXZ",NameVar) == 0){
    ind = SFindNode(indx);
    i = ind.x + ind.y * SDMGeom->HALO_NodeX() + ind.z * SDMGeom->HALO_NodeX() * \
      SDMGeom->HALO_NodeY();
    sxz[i] = Val;
  }

  if (strcmp("SYZ",NameVar) == 0){
    ind = SFindNode(indx);
    i = ind.x + ind.y * SDMGeom->HALO_NodeX() + ind.z * SDMGeom->HALO_NodeX() * \
      SDMGeom->HALO_NodeY();
    syz[i] = Val;
  }

  if (strcmp("VX",NameVar) == 0){
    ind = SFindNode(indx);
    i = ind.x + ind.y * SDMGeom->HALO_NodeX() + ind.z * SDMGeom->HALO_NodeX() * \
      SDMGeom->HALO_NodeY();
    vx[i] = Val;
  }

  if (strcmp("VY",NameVar) == 0){
    ind = SFindNode(indx);
    i = ind.x + ind.y * SDMGeom->HALO_NodeX() + ind.z * SDMGeom->HALO_NodeX() * \
      SDMGeom->HALO_NodeY();
    vy[i] = Val;
  }

  if (strcmp("VZ",NameVar) == 0){
    ind = SFindNode(indx);
    i = ind.x + ind.y * SDMGeom->HALO_NodeX() + ind.z * SDMGeom->HALO_NodeX() * \
      SDMGeom->HALO_NodeY();
    vz[i] = Val;
  }
  
}


Dfloat SDM::GetVal(VecI indx, char *NameVar){
  VecI ind;
  Dfloat Val;
  int i;
  
   if (strcmp("SXX",NameVar) == 0){
    ind = SFindNode(indx);
    i = ind.x + ind.y * SDMGeom->HALO_NodeX() + ind.z * SDMGeom->HALO_NodeX() * \
      SDMGeom->HALO_NodeY();
    Val = sxx[i];
  }
  if (strcmp("SYY",NameVar) == 0){
    ind = SFindNode(indx);
    i = ind.x + ind.y * SDMGeom->HALO_NodeX() + ind.z * SDMGeom->HALO_NodeX() * \
      SDMGeom->HALO_NodeY();
    Val = syy[i];
  }

  if (strcmp("SZZ",NameVar) == 0){
    ind = SFindNode(indx);
    i = ind.x + ind.y * SDMGeom->HALO_NodeX() + ind.z * SDMGeom->HALO_NodeX() * \
      SDMGeom->HALO_NodeY();
    Val = szz[i];
  }

  if (strcmp("SXY",NameVar) == 0){
    ind = SFindNode(indx);
    i = ind.x + ind.y * SDMGeom->HALO_NodeX() + ind.z * SDMGeom->HALO_NodeX() * \
      SDMGeom->HALO_NodeY();
    Val = sxy[i];
  }

  if (strcmp("SXZ",NameVar) == 0){
    ind = SFindNode(indx);
    i = ind.x + ind.y * SDMGeom->HALO_NodeX() + ind.z * SDMGeom->HALO_NodeX() * \
      SDMGeom->HALO_NodeY();
    Val = sxz[i];
  }

  if (strcmp("SYZ",NameVar) == 0){
    ind = SFindNode(indx);
    i = ind.x + ind.y * SDMGeom->HALO_NodeX() + ind.z * SDMGeom->HALO_NodeX() * \
      SDMGeom->HALO_NodeY();
    Val = syz[i];
  }

  if (strcmp("VX",NameVar) == 0){
    ind = SFindNode(indx);
    i = ind.x + ind.y * SDMGeom->HALO_NodeX() + ind.z * SDMGeom->HALO_NodeX() * \
      SDMGeom->HALO_NodeY();
    Val = vx[i];
  }

  if (strcmp("VY",NameVar) == 0){
    ind = SFindNode(indx);
    i = ind.x + ind.y * SDMGeom->HALO_NodeX() + ind.z * SDMGeom->HALO_NodeX() * \
      SDMGeom->HALO_NodeY();
    Val = vy[i];
  }

  if (strcmp("VZ",NameVar) == 0){
    ind = SFindNode(indx);
    i = ind.x + ind.y * SDMGeom->HALO_NodeX() + ind.z * SDMGeom->HALO_NodeX() * \
      SDMGeom->HALO_NodeY();
    Val = vz[i];
  }
  
}

void SDM:: FD_SII(VecI Init,VecI Iend){
  VecI Lindx, Gindx;
  Dfloat df_dI,df_dJ,df_dK;
  Dfloat mu_avg,lamb_avg;
  


  for (int iz=Init.z + HALO.z; iz<Iend.z + HALO.z; ++iz){
    for (int iy=Init.y + HALO.y; iy<Iend.y + HALO.y; ++iy){
      for (int ix=Init.x + HALO.x; ix<Iend.x + HALO.x; ++ix){


	Lindx = {ix,iy,iz};
	Gindx = Loc2Glo(Lindx);

	df_dI = ( C1 * (vx[IJK(ix+1,iy,iz)] - vx[IJK(ix,iy,iz)]) - \
		  C0 * (vx[IJK(ix+2,iy,iz)] - vx[IJK(ix-1,iy,iz)]) ) / SDMGeom->Dx();
	
	df_dJ = ( C1 * (vy[IJK(ix,iy,iz)] - vy[IJK(ix,iy-1,iz)]) - \
		  C0 * (vy[IJK(ix,iy+1,iz)] - vy[IJK(ix,iy-2,iz)]) ) / SDMGeom->Dy();
	
	df_dK = ( C1 * (vz[IJK(ix,iy,iz)] - vz[IJK(ix,iy,iz-1)]) - \
		  C0 * (vz[IJK(ix,iy,iz+1)] - vz[IJK(ix,iy,iz-2)]) ) / SDMGeom->Dz();


	dvx_dx[IJK(ix,iy,iz)] = pml_x->B(Gindx.x) * dvx_dx[IJK(ix,iy,iz)] \
	  + pml_x->A(Gindx.x) * df_dI;

	dvy_dy[IJK(ix,iy,iz)] = pml_y->B_HALF(Gindx.y) * dvy_dy[IJK(ix,iy,iz)] \
	  + pml_y->A_HALF(Gindx.y) * df_dJ;

	dvz_dz[IJK(ix,iy,iz)] = pml_z->B_HALF(Gindx.z) * dvz_dz[IJK(ix,iy,iz)]  \
	  + pml_z->A_HALF(Gindx.z) * df_dK;

	df_dI += df_dI;
	df_dJ += df_dJ;
	df_dK += df_dK;

	// Average properties
	
	mu_avg = (mu[IJK(ix,iy,iz)] + mu[IJK(ix+1,iy,iz)]) / 2.0;
	lamb_avg = (lamb[IJK(ix,iy,iz)] + lamb[IJK(ix+1,iy,iz)]) / 2.0;
	
	// SXX

	sxx[IJK(ix,iy,iz)] = sxx[IJK(ix,iy,iz)] + \
	  (dt * (lamb_avg + 2.0 * mu_avg) * df_dI) + \
	  dt * lamb_avg *  (df_dJ + df_dK);

	// SYY
	
	syy[IJK(ix,iy,iz)] = syy[IJK(ix,iy,iz)] + \
	  (dt * (lamb_avg + 2.0 * mu_avg) * df_dJ) + \
	  dt * lamb_avg *  (df_dI + df_dK);

	// SZZ
	
	szz[IJK(ix,iy,iz)] = szz[IJK(ix,iy,iz)] + \
	  (dt * (lamb_avg + 2.0 * mu_avg) * df_dK) +  \
	  dt * lamb_avg *  (df_dJ + df_dI);


	

      }
    }
  }

}


void SDM::FD_VX(VecI Init,VecI Iend){

  VecI Lindx,Gindx;
  Dfloat df_dI,df_dJ,df_dK;
  


  for (int iz=Init.z + HALO.z; iz<Iend.z + HALO.z; ++iz){
    for (int iy=Init.y + HALO.y; iy<Iend.y + HALO.y; ++iy){
      for (int ix=Init.x + HALO.x; ix<Iend.x + HALO.x; ++ix){


	Lindx = {ix,iy,iz};
	Gindx = Loc2Glo(Lindx);

	df_dI = ( C1 * (sxx[IJK(ix,iy,iz)] - sxx[IJK(ix-1,iy,iz)]) - \
		  C0 * (sxx[IJK(ix+1,iy,iz)] - sxx[IJK(ix-2,iy,iz)]) ) / SDMGeom->Dx();
	
	df_dJ = ( C1 * (sxy[IJK(ix,iy,iz)] - sxy[IJK(ix,iy-1,iz)]) - \
		  C0 * (sxy[IJK(ix,iy+1,iz)] - sxy[IJK(ix,iy-2,iz)]) ) / SDMGeom->Dy();
	
	df_dK = ( C1 * (sxz[IJK(ix,iy,iz)] - sxz[IJK(ix,iy,iz-1)]) - \
		  C0 * (sxz[IJK(ix,iy,iz+1)] - sxz[IJK(ix,iy,iz-2)]) ) / SDMGeom->Dz();


	dsxx_dx[IJK(ix,iy,iz)] = pml_x->B_HALF(Gindx.x) * dsxx_dx[IJK(ix,iy,iz)] \
	  + pml_x->A_HALF(Gindx.x) * df_dI;

	dsxy_dy[IJK(ix,iy,iz)] = pml_y->B_HALF(Gindx.y) * dsxy_dy[IJK(ix,iy,iz)] \
	  + pml_y->A_HALF(Gindx.y) * df_dJ;

	dsxz_dz[IJK(ix,iy,iz)] = pml_z->B_HALF(Gindx.z) * dsxz_dz[IJK(ix,iy,iz)]  \
	  + pml_z->A_HALF(Gindx.z) * df_dK;

	df_dI += df_dI;
	df_dJ += df_dJ;
	df_dK += df_dK;


	vx[IJK(ix,iy,iz)] = vx[IJK(ix,iy,iz)] + (dt / rho[IJK(ix,iy,iz)]) * \
	  (df_dI + df_dJ + df_dK);


	
      }
    }
  }


  

  
}


void SDM::FD_VY(VecI Init,VecI Iend){

  VecI Lindx,Gindx;
  Dfloat df_dI,df_dJ,df_dK;
  Dfloat rho_avg;
  


  for (int iz=Init.z + HALO.z; iz<Iend.z + HALO.z; ++iz){
    for (int iy=Init.y + HALO.y; iy<Iend.y + HALO.y; ++iy){
      for (int ix=Init.x + HALO.x; ix<Iend.x + HALO.x; ++ix){


	Lindx = {ix,iy,iz};
	Gindx = Loc2Glo(Lindx);

	df_dI = ( C1 * (sxy[IJK(ix+1,iy,iz)] - sxy[IJK(ix,iy,iz)]) - \
		  C0 * (sxy[IJK(ix+2,iy,iz)] - sxy[IJK(ix-1,iy,iz)]) ) / SDMGeom->Dx();
	
	df_dJ = ( C1 * (syy[IJK(ix,iy+1,iz)] - syy[IJK(ix,iy,iz)]) - \
		  C0 * (syy[IJK(ix,iy+2,iz)] - syy[IJK(ix,iy-1,iz)]) ) / SDMGeom->Dy();
	
	df_dK = ( C1 * (syz[IJK(ix,iy,iz)] - syz[IJK(ix,iy,iz-1)]) - \
		  C0 * (syz[IJK(ix,iy,iz+1)] - syz[IJK(ix,iy,iz-2)]) ) / SDMGeom->Dz();


	dsxy_dx[IJK(ix,iy,iz)] = pml_x->B(Gindx.x) * dsxy_dx[IJK(ix,iy,iz)] \
	  + pml_x->A(Gindx.x) * df_dI;

	dsyy_dy[IJK(ix,iy,iz)] = pml_y->B(Gindx.y) * dsyy_dy[IJK(ix,iy,iz)] \
	  + pml_y->A(Gindx.y) * df_dJ;

	dszz_dz[IJK(ix,iy,iz)] = pml_z->B_HALF(Gindx.z) * dszz_dz[IJK(ix,iy,iz)]  \
	  + pml_z->A_HALF(Gindx.z) * df_dK;

	df_dI += df_dI;
	df_dJ += df_dJ;
	df_dK += df_dK;


	rho_avg = ( rho[IJK(ix,iy,iz)] + rho[IJK(ix+1,iy,iz)] + \
		rho[IJK(ix,iy+1,iz)] + rho[IJK(ix+1,iy+1,iz)]) / 4.0;

	vy[IJK(ix,iy,iz)] = vy[IJK(ix,iy,iz)] + (dt / rho[IJK(ix,iy,iz)]) * \
	  (df_dI + df_dJ + df_dK);


	
      }
    }
  }


  

  
}



void SDM::FD_VZ(VecI Init,VecI Iend){

  VecI Lindx,Gindx;
  Dfloat df_dI,df_dJ,df_dK;
  Dfloat rho_avg;
  


  for (int iz=Init.z + HALO.z; iz<Iend.z + HALO.z; ++iz){
    for (int iy=Init.y + HALO.y; iy<Iend.y + HALO.y; ++iy){
      for (int ix=Init.x + HALO.x; ix<Iend.x + HALO.x; ++ix){


	Lindx = {ix,iy,iz};
	Gindx = Loc2Glo(Lindx);

	df_dI = ( C1 * (sxz[IJK(ix+1,iy,iz)] - sxz[IJK(ix,iy,iz)]) - \
		  C0 * (sxz[IJK(ix+2,iy,iz)] - sxz[IJK(ix-1,iy,iz)]) ) / SDMGeom->Dx();
	
	df_dJ = ( C1 * (syz[IJK(ix,iy,iz)] - syz[IJK(ix,iy-1,iz)]) - \
		  C0 * (syz[IJK(ix,iy+1,iz)] - syz[IJK(ix,iy-2,iz)]) ) / SDMGeom->Dy();
	
	df_dK = ( C1 * (szz[IJK(ix,iy,iz+1)] - szz[IJK(ix,iy,iz)]) - \
		  C0 * (szz[IJK(ix,iy,iz+2)] - szz[IJK(ix,iy,iz-1)]) ) / SDMGeom->Dz();


	dsxz_dx[IJK(ix,iy,iz)] = pml_x->B(Gindx.x) * dsxz_dx[IJK(ix,iy,iz)] \
	  + pml_x->A(Gindx.x) * df_dI;

	dsyz_dy[IJK(ix,iy,iz)] = pml_y->B_HALF(Gindx.y) * dsyz_dy[IJK(ix,iy,iz)] \
	  + pml_y->A_HALF(Gindx.y) * df_dJ;

	dszz_dz[IJK(ix,iy,iz)] = pml_z->B(Gindx.z) * dszz_dz[IJK(ix,iy,iz)]  \
	  + pml_z->A(Gindx.z) * df_dK;

	df_dI += df_dI;
	df_dJ += df_dJ;
	df_dK += df_dK;

	rho_avg = ( rho[IJK(ix,iy,iz)] + rho[IJK(ix+1,iy,iz)] + \
		rho[IJK(ix,iy,iz+1)] + rho[IJK(ix+1,iy,iz+1)]) / 4.0;

	vz[IJK(ix,iy,iz)] = vz[IJK(ix,iy,iz)] + (dt / rho_avg) * \
	  (df_dI + df_dJ + df_dK);


	
      }
    }
  }

  
}



void SDM::FD_SXY(VecI Init,VecI Iend){

  VecI Lindx,Gindx;
  Dfloat df_dI,df_dJ,df_dK;
  Dfloat mu_avg;


  for (int iz=Init.z + HALO.z; iz<Iend.z + HALO.z; ++iz){
    for (int iy=Init.y + HALO.y; iy<Iend.y + HALO.y; ++iy){
      for (int ix=Init.x + HALO.x; ix<Iend.x + HALO.x; ++ix){


	Lindx = {ix,iy,iz};
	Gindx = Loc2Glo(Lindx);

	df_dJ = ( C1 * (vx[IJK(ix,iy+1,iz)] - vx[IJK(ix,iy,iz)]) - \
		  C0 * (vx[IJK(ix,iy+2,iz)] - vx[IJK(ix,iy-1,iz)]) ) / SDMGeom->Dy();
	
	df_dI = ( C1 * (vy[IJK(ix,iy,iz)] - vy[IJK(ix-1,iy,iz)]) - \
		  C0 * (vy[IJK(ix+1,iy,iz)] - vy[IJK(ix-2,iy,iz)]) ) / SDMGeom->Dx();
	

	dvy_dx[IJK(ix,iy,iz)] = pml_x->B_HALF(Gindx.x) * dvy_dx[IJK(ix,iy,iz)] \
	  + pml_x->A_HALF(Gindx.x) * df_dI;

	dvx_dy[IJK(ix,iy,iz)] = pml_y->B(Gindx.y) * dvx_dy[IJK(ix,iy,iz)] \
	  + pml_y->A(Gindx.y) * df_dJ;


	mu_avg = (mu[IJK(ix,iy,iz)] + mu[IJK(ix,iy+1,iz)]) / 2.0;


	df_dI += df_dI;
	df_dJ += df_dJ;


	sxy[IJK(ix,iy,iz)] = sxy[IJK(ix,iy,iz)] + (dt * mu_avg) * \
	  (df_dI + df_dJ);


	
      }
    }
  }

}

void SDM::FD_SXZ(VecI Init,VecI Iend){

  VecI Lindx,Gindx;
  Dfloat df_dI,df_dJ,df_dK;
  Dfloat mu_avg;


  for (int iz=Init.z + HALO.z; iz<Iend.z + HALO.z; ++iz){
    for (int iy=Init.y + HALO.y; iy<Iend.y + HALO.y; ++iy){
      for (int ix=Init.x + HALO.x; ix<Iend.x + HALO.x; ++ix){


	Lindx = {ix,iy,iz};
	Gindx = Loc2Glo(Lindx);

	df_dI = ( C1 * (vz[IJK(ix,iy,iz)] - vz[IJK(ix-1,iy,iz)]) - \
		  C0 * (vz[IJK(ix+1,iy,iz)] - vz[IJK(ix-2,iy,iz)]) ) / SDMGeom->Dx();
	
	df_dK = ( C1 * (vx[IJK(ix,iy,iz+1)] - vx[IJK(ix,iy,iz)]) - \
		  C0 * (vx[IJK(ix,iy,iz+2)] - vx[IJK(ix,iy,iz-1)]) ) / SDMGeom->Dz();
	

	dvz_dx[IJK(ix,iy,iz)] = pml_x->B_HALF(Gindx.x) * dvz_dx[IJK(ix,iy,iz)] \
	  + pml_x->A_HALF(Gindx.x) * df_dI;

	dvx_dz[IJK(ix,iy,iz)] = pml_z->B(Gindx.y) * dvx_dz[IJK(ix,iy,iz)] \
	  + pml_z->A(Gindx.y) * df_dK;


	mu_avg = (mu[IJK(ix,iy,iz)] + mu[IJK(ix,iy,iz+1)]) / 2.0;


	df_dI += df_dI;
	df_dK += df_dK;


	sxz[IJK(ix,iy,iz)] = sxz[IJK(ix,iy,iz)] + (dt * mu_avg) * \
	  (df_dI + df_dK);


	
      }
    }
  }



  

  
}


void SDM::FD_SYZ(VecI Init,VecI Iend){

  VecI Lindx,Gindx;
  Dfloat df_dI,df_dJ,df_dK;
  Dfloat mu_avg;


  for (int iz=Init.z + HALO.z; iz<Iend.z + HALO.z; ++iz){
    for (int iy=Init.y + HALO.y; iy<Iend.y + HALO.y; ++iy){
      for (int ix=Init.x + HALO.x; ix<Iend.x + HALO.x; ++ix){


	Lindx = {ix,iy,iz};
	Gindx = Loc2Glo(Lindx);

	df_dJ = ( C1 * (vz[IJK(ix,iy+1,iz)] - vz[IJK(ix,iy,iz)]) - \
		  C0 * (vz[IJK(ix,iy+2,iz)] - vz[IJK(ix,iy-1,iz)]) ) / SDMGeom->Dy();
	
	df_dK = ( C1 * (vy[IJK(ix,iy,iz+1)] - vy[IJK(ix,iy,iz)]) - \
		  C0 * (vy[IJK(ix,iy,iz+2)] - vy[IJK(ix,iy,iz-1)]) ) / SDMGeom->Dz();
	

	dvz_dy[IJK(ix,iy,iz)] = pml_x->B(Gindx.x) * dvz_dy[IJK(ix,iy,iz)] \
	  + pml_x->A_HALF(Gindx.x) * df_dJ;

	dvy_dz[IJK(ix,iy,iz)] = pml_z->B(Gindx.y) * dvy_dz[IJK(ix,iy,iz)] \
	  + pml_z->A(Gindx.y) * df_dK;


	mu_avg = (mu[IJK(ix,iy,iz)] + mu[IJK(ix+1,iy,iz)] + \
		mu[IJK(ix,iy+1,iz)] + mu[IJK(ix+1,iy+1,iz)] + \
		mu[IJK(ix,iy,iz+1)] + mu[IJK(ix+1,iy,iz+1)] + \
		mu[IJK(ix,iy+1,iz+1)] + mu[IJK(ix+1,iy+1,iz+1)]) / 8.0;


	df_dJ += df_dJ;
	df_dK += df_dK;


	syz[IJK(ix,iy,iz)] = syz[IJK(ix,iy,iz)] + (dt * mu_avg) * \
	  (df_dJ + df_dK);


	
      }
    }
  }

  
}






