/*
 *      Author: mverzett
 */

#ifndef DEEPNTUPLES_DEEPNTUPLIZER_INTERFACE_HELPERS_H_
#define DEEPNTUPLES_DEEPNTUPLIZER_INTERFACE_HELPERS_H_
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Electron.h"

namespace deep_ntuples {
    enum JetFlavor {UNDEFINED, G, UD, S, C, GCC, CC, B, GBB, BB, LeptonicB, LeptonicB_C, TAU, TAUTAU,
        TAUH, TAUM, TAUE, TAUHTAUH, TAUHTAUM, TAUHTAUE, TAUMTAUM, TAUMTAUE, TAUETAUE,
        TAUH0, TAUH1, TAUH10, TAUH0TAUH0, TAUH0TAUH1, TAUH0TAUH10, TAUH1TAUH1, TAUH1TAUH10, TAUH10TAUH10,
        TAUH0TAUM, TAUH1TAUM, TAUH10TAUM, TAUH0TAUE, TAUH1TAUE, TAUH10TAUE
    };
    JetFlavor jet_flavour(const pat::Jet& jet,
            const std::vector<reco::GenParticle>& gToBB,
            const std::vector<reco::GenParticle>& gToCC,
            const std::vector<reco::GenParticle>& neutrinosLepB,
            const std::vector<reco::GenParticle>& neutrinosLepB_C,
            const std::vector<reco::GenParticle>& alltaus,
            bool usePhysForLightAndUndefined=false);
    std::vector<std::size_t> jet_muonsIds(const pat::Jet& jet, const std::vector<pat::Muon>& event_muons); 
    std::vector<std::size_t> jet_electronsIds(const pat::Jet& jet, const std::vector<pat::Electron>& event_electrons); 
}

#endif //DEEPNTUPLES_DEEPNTUPLIZER_INTERFACE_HELPERS_H_

#include <tuple>
#include "DataFormats/JetReco/interface/Jet.h"
namespace yuta{

std::tuple<int, int, int, float, float, float, float>
calcVariables(const reco::Jet *jet);


}
