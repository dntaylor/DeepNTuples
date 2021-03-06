#include "../interface/helpers.h"
#include <vector>

namespace deep_ntuples {

std::vector<std::size_t> jet_muonsIds(const pat::Jet& jet, const std::vector<pat::Muon>& event_muons) {
    std::vector <std::size_t> muonsIds;
    for (std::size_t i = 0; i < event_muons.size(); i++) {
        const auto & muon = event_muons.at(i);
        if(reco::deltaR(muon.eta(),muon.phi(),jet.eta(),jet.phi()) < 0.4) muonsIds.emplace_back(i);
    }
    return muonsIds;
}

std::vector<std::size_t> jet_electronsIds(const pat::Jet& jet, const std::vector<pat::Electron>& event_electrons) {
    std::vector <std::size_t> electronsIds;
    for (std::size_t i = 0; i < event_electrons.size(); i++) {
        const auto & electron = event_electrons.at(i);
        if(reco::deltaR(electron.eta(),electron.phi(),jet.eta(),jet.phi()) < 0.4) electronsIds.emplace_back(i);
    }
    return electronsIds;
}

JetFlavor jet_flavour(const pat::Jet& jet,
        const std::vector<reco::GenParticle>& gToBB,
        const std::vector<reco::GenParticle>& gToCC,
        const std::vector<reco::GenParticle>& neutrinosLepB,
        const std::vector<reco::GenParticle>& neutrinosLepB_C,
        const std::vector<reco::GenParticle>& alltaus,
        bool usePhysForLightAndUndefined) { 
    int hflav = abs(jet.hadronFlavour());
    int pflav = abs(jet.partonFlavour());
    int physflav = 0;
    if(jet.genParton()) physflav=abs(jet.genParton()->pdgId());
    std::size_t nbs = jet.jetFlavourInfo().getbHadrons().size();
    std::size_t ncs = jet.jetFlavourInfo().getcHadrons().size();

    unsigned int nbFromGSP(0);
    for (reco::GenParticle p : gToBB) {
        double dr(reco::deltaR(jet, p));
        if (dr < 0.4) ++nbFromGSP;
    }

    unsigned int ncFromGSP(0);
    for (reco::GenParticle p : gToCC) {
        double dr(reco::deltaR(jet, p));
        if (dr < 0.4) ++ncFromGSP;
    }

    //std::cout << " jet pt = " << jet.pt() << " hfl = " << hflav << " pfl = " << pflav << " genpart = " << physflav
            //  << " nbFromGSP = " << nbFromGSP << " ncFromGSP = " << ncFromGSP
    //  << " nBhadrons " << nbs << " nCHadrons " << ncs << std::endl;

    if(hflav == 5) { //B jet
        if(nbs > 1) {
            if (nbFromGSP > 0) return JetFlavor::GBB;
            else return JetFlavor::BB;
        }
        else if(nbs == 1) {
            for (std::vector<reco::GenParticle>::const_iterator it = neutrinosLepB.begin(); it != neutrinosLepB.end(); ++it){
                if(reco::deltaR(it->eta(),it->phi(),jet.eta(),jet.phi()) < 0.4) {
                    return JetFlavor::LeptonicB;
                }
            }
            for (std::vector<reco::GenParticle>::const_iterator it = neutrinosLepB_C.begin(); it != neutrinosLepB_C.end(); ++it){
                if(reco::deltaR(it->eta(),it->phi(),jet.eta(),jet.phi()) < 0.4) {
                    return JetFlavor::LeptonicB_C;
                }
            }
            return JetFlavor::B;
        }
        else {
            if(usePhysForLightAndUndefined){
                if(physflav == 21) return JetFlavor::G;
                else if(physflav == 3) return JetFlavor::S;
                else if(physflav == 2 || physflav ==1) return JetFlavor::UD;
                else return JetFlavor::UNDEFINED;
            }
            else return JetFlavor::UNDEFINED;
        }
    }
    else if(hflav == 4) { //C jet
        if (ncs > 1) {
            if (ncFromGSP > 0) return JetFlavor::GCC;
            else return JetFlavor::CC;
        }
        else return JetFlavor::C;
    }
    else { //not a heavy jet
        if(alltaus.size()>0){ //check for tau in a simplistic way
            int ntauhaddm0 = 0;
            int ntauhaddm1 = 0;
            int ntauhaddm10 = 0;
            int ntaumu = 0;
            int ntaue = 0;
            float drMatch = 0.4;
            for(const auto& p:alltaus){
                size_t ndau=p.numberOfDaughters();
                int npi0 = 0;
                int nch = 0;
                for(size_t i=0;i<ndau;i++){
                    const reco::Candidate* dau=p.daughter(i);
                    int daupid=std::abs(dau->pdgId());
                    int dauch = dau->charge();
                    if (reco::deltaR(*dau,jet) > drMatch) continue;
                    if(daupid == 13){
                        ntaumu++;
                    }
                    else if(daupid == 11){
                        ntaue++;
                    }
                    else if(daupid != 12 && daupid!=14 && daupid!=16) {
                        if(dauch) {
                            nch++;
                        }
                        else {
                            npi0++;
                        }
                    }
                }
                if (nch==1) {
                    if (npi0) {
                        ntauhaddm1++;
                    }
                    else {
                        ntauhaddm0++;
                    }
                }
                else if(nch==3) {
                    ntauhaddm10++;
                }
            }
            int ntauhad = ntauhaddm0+ntauhaddm1+ntauhaddm10;
            int ntau = ntauhad+ntaue+ntaumu;
            bool doSimple = false;
            bool doEM = true;
            bool doDM = false;
            if (doSimple) {
                if (doEM) {
                    if (ntau>1) return JetFlavor::TAUTAU;
                }
                else {
                    if (ntauhad>1) return JetFlavor::TAUTAU;
                }
                if (ntauhad)   return JetFlavor::TAU;
            }
            else {
                // TAU TAU
                if (doDM) {
                    if (ntauhaddm0>1)                    return JetFlavor::TAUH0TAUH0;
                    if (ntauhaddm1>1)                    return JetFlavor::TAUH1TAUH1;
                    if (ntauhaddm10>1)                   return JetFlavor::TAUH10TAUH10;
                    if (ntauhaddm0==1 && ntauhaddm1==1)  return JetFlavor::TAUH0TAUH1;
                    if (ntauhaddm0==1 && ntauhaddm10==1) return JetFlavor::TAUH0TAUH10;
                    if (ntauhaddm1==1 && ntauhaddm10==1) return JetFlavor::TAUH1TAUH10;
                    if (doEM) {
                        if (ntauhaddm0==1 && ntaumu==1)  return JetFlavor::TAUH0TAUM;
                        if (ntauhaddm1==1 && ntaumu==1)  return JetFlavor::TAUH1TAUM;
                        if (ntauhaddm10==1 && ntaumu==1) return JetFlavor::TAUH10TAUM;
                        if (ntauhaddm0==1 && ntaue==1)   return JetFlavor::TAUH0TAUE;
                        if (ntauhaddm1==1 && ntaue==1)   return JetFlavor::TAUH1TAUE;
                        if (ntauhaddm10==1 && ntaue==1)  return JetFlavor::TAUH10TAUE;
                    }
                }
                else {
                    if (ntauhad>1)                       return JetFlavor::TAUHTAUH;
                    if (doEM) {
                        if (ntauhad==1 && ntaumu==1)     return JetFlavor::TAUHTAUM;
                        if (ntauhad==1 && ntaue==1)      return JetFlavor::TAUHTAUE;
                    }
                }
                if (doEM) {
                    if (ntaumu>1)                        return JetFlavor::TAUMTAUM;
                    if (ntaue>1)                         return JetFlavor::TAUETAUE;
                    if (ntaumu==1 && ntaue==1)           return JetFlavor::TAUMTAUE;
                }
                // TAU
                if (doDM) {
                    if (ntauhaddm0)                      return JetFlavor::TAUH0;
                    if (ntauhaddm1)                      return JetFlavor::TAUH1;
                    if (ntauhaddm10)                     return JetFlavor::TAUH10;
                }
                else {
                    if (ntauhad)                         return JetFlavor::TAUH;
                }
                if (doEM) {
                   if (ntaumu)                           return JetFlavor::TAUM;
                   if (ntaue)                            return JetFlavor::TAUE;
                }
            }

        }
        if(std::abs(pflav) == 4 || std::abs(pflav) == 5 || nbs || ncs) {
            if(usePhysForLightAndUndefined){
                if(physflav == 21) return JetFlavor::G;
                else if(physflav == 3) return JetFlavor::S;
                else if(physflav == 2 || physflav ==1) return JetFlavor::UD;
                else return JetFlavor::UNDEFINED;
            }
            else return JetFlavor::UNDEFINED;
        }
        else if(usePhysForLightAndUndefined){
            if(physflav == 21) return JetFlavor::G;
            else if(physflav == 3) return JetFlavor::S;
            else if(physflav == 2 || physflav ==1) return JetFlavor::UD;
            else return JetFlavor::UNDEFINED;
        }
        else {
            if(pflav == 21) return JetFlavor::G;
            else if(pflav == 3) return JetFlavor::S;
            else if(pflav == 2 || pflav ==1) return JetFlavor::UD;
            else return JetFlavor::UNDEFINED;
        }
    }
    return JetFlavor::UNDEFINED;
}
}

namespace yuta{

std::tuple<int, int, int, float, float, float, float> calcVariables(const reco::Jet *jet){
    float sum_weight = 0., sum_deta = 0., sum_dphi = 0., sum_deta2 = 0., sum_dphi2 = 0., sum_detadphi = 0., sum_pt = 0.;
    int multiplicity = 0;
    int charged_multiplicity = 0, neutral_multiplicity = 0;
    float pt_dr_log = 0;

    bool useQC=false;

    //  float sum_pt1 = 0;
    //  float sum_pt2 = 0;

    //  TVector3 _pull1(0, 0, 0);
    //  TVector3 _pull2(0, 0, 0);
    //
    std::vector<const pat::PackedCandidate*> constituents;
    for ( unsigned ida = 0; ida < jet->numberOfDaughters(); ++ida ) {
      reco::Candidate const * cand = jet->daughter(ida);
      if ( cand->numberOfDaughters() == 0 )
        constituents.push_back( dynamic_cast<const pat::PackedCandidate*>(cand) ) ;
      else {
        for ( unsigned jda = 0; jda < cand->numberOfDaughters(); ++jda ) {
          reco::Candidate const * cand2 = cand->daughter(jda);
          constituents.push_back( dynamic_cast<const pat::PackedCandidate*>(cand2) );
        }
      }
    }

    //Loop over the jet constituents
    for (auto dau : constituents){
        const pat::PackedCandidate* daughter = dynamic_cast<const pat::PackedCandidate*>(dau);
        if(daughter){                                        //packed candidate situation
            auto part = static_cast<const pat::PackedCandidate*>(daughter);

            //      std::cout << "daughter pdg Id = " << daughter->pdgId() << std::endl;
            //      std::cout << "daughter pdg Id = " << daughter->particleId() << std::endl;

            if(part->charge()){

                if(!(part->fromPV() > 1 && part->trackHighPurity())) continue;
                if(useQC){
                    if((part->dz()*part->dz())/(part->dzError()*part->dzError()) > 25.) continue;
                    if((part->dxy()*part->dxy())/(part->dxyError()*part->dxyError()) < 25.){
                        ++multiplicity;
                        ++charged_multiplicity;
                    }
                } else{
                    ++multiplicity;
                    ++charged_multiplicity;

                    //      Int_t pdg = abs(part->pdgId());
                    //      std::cout << "charged : " << pdg << std::endl;
                    //std::cout << "charged PDG = " << pdg << std::endl;

                };

            } else {
                if(part->pt() < 1.0) continue;
                ++multiplicity;
                ++neutral_multiplicity;

                //  Int_t pdg = abs(part->pdgId());
                //  std::cout << "neutral : " << pdg << std::endl;

                //  if(pdg==211){;}
                //  else if(pdg==310 || pdg==130){;}
                //  else if(pdg==22){;}
                //  else if(pdg<=9 || pdg==21){;}
                //  else if(pdg>=11 && pdg<=16){;}
                //  else{
                //    //      std::cout << "neutral PDG = " << pdg << std::endl;
                //    //      std::cout << "What !!!!!!!!!!!!!!!!!!!!!!!! " << pdg << std::endl;
                //  }


                //  if(pdg<=9 || pdg==21){std::cout << "NOT possible " << pdg << std::endl;}
                //  else if(pdg>=11 && pdg<=16){std::cout << "NOT possible " << pdg << std::endl;}
                //  else{
                //    std::cout << "included :" << pdg << std::endl;
                //    ++neutral_multiplicity;
                //  }


            }

            //      if(part->pt() > max_pt) max_pt = part->pt();
            //      sum_pt1 += part->pt();
            //      sum_pt2 += part->pt()*part->pt();

            //      Int_t pdg = abs(part->pdgId());

            //      if(part->pt() > 1.0){

            //      if(pdg==211){ pion_multiplicity++;}
            //      else if(pdg==310 || pdg==130){ kaon_multiplicity++;}
            //      else if(pdg==22){photon_multiplicity++;}
            //      else if(pdg<=9 || pdg==21){;}
            //      else if(pdg>=11 && pdg<=16){;}
            //      else std::cout << "What !!!!!!!!!!!!!!!!!!!!!!!! " << pdg << std::endl;
            //      }


            float dr = reco::deltaR(*jet, *part);

            //      pt_dr += (part->pt()/dr);
            //      pt_dr2 += (part->pt()/(dr*dr));
            pt_dr_log += std::log(part->pt()/dr);

            //      TVector3 _jet(jet->px(), jet->py(), 0);
            //      TVector3 _part(part->px(), part->py(), 0);

            //      _pull1 += (_part - _jet);
            //      _pull2 += (_part - _jet);
            //
            //      _pull1 *= part->pt()*dr;
            //      _pull2 *= part->pt()*part->pt()*dr;

            //      TVector3 _v = _jet.Unit().Cross(_part);

            //      if(_v.Mag()!=0 && dr!=0){
            //  ptrel += std::log(1/_v.Mag());
            //  ptrel_dr += std::log(1/(_v.Mag()*dr));
            //      }else{
            //  std::cout << "Either ptrel = " << _v.Mag() << " or dR = " << dr << " is 0 !!!" << std::endl;
            //      }
        }

        float deta   = daughter->eta() - jet->eta();
        float dphi   = reco::deltaPhi(daughter->phi(), jet->phi());
        float partPt = daughter->pt();
        float weight = partPt*partPt;

        sum_weight   += weight;
        sum_pt       += partPt;
        sum_deta     += deta*weight;
        sum_dphi     += dphi*weight;
        sum_deta2    += deta*deta*weight;
        sum_detadphi += deta*dphi*weight;
        sum_dphi2    += dphi*dphi*weight;
    }

    //Calculate axis2 and ptD
    float a = 0., b = 0., c = 0.;
    float ave_deta = 0., ave_dphi = 0., ave_deta2 = 0., ave_dphi2 = 0.;
    if(sum_weight > 0){
        ave_deta  = sum_deta/sum_weight;
        ave_dphi  = sum_dphi/sum_weight;
        ave_deta2 = sum_deta2/sum_weight;
        ave_dphi2 = sum_dphi2/sum_weight;
        a         = ave_deta2 - ave_deta*ave_deta;
        b         = ave_dphi2 - ave_dphi*ave_dphi;
        c         = -(sum_detadphi/sum_weight - ave_deta*ave_dphi);
    }
    float delta = sqrt(fabs((a-b)*(a-b)+4*c*c));
    float axis1 = (a+b+delta > 0 ?  sqrt(0.5*(a+b+delta)) : 0);
    float axis2 = (a+b-delta > 0 ?  sqrt(0.5*(a+b-delta)) : 0);
    float ptD   = (sum_weight > 0 ? sqrt(sum_weight)/sum_pt : 0);

    //  float pull1 = _pull1.Mag()/sum_pt1;
    //  float pull2 = _pull2.Mag()/sum_pt2;

    return std::make_tuple(multiplicity, charged_multiplicity, neutral_multiplicity, ptD, axis1, axis2, pt_dr_log);
}


}

