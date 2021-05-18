// _bladeText->setText(to_string(_resources[0]));
//      _flourishText->setText(to_string(_resources[1]));
//      _lungeText->setText(to_string(_resources[2]));
//      _brawnText->setText(to_string(_resources[3]));
#include "ResourceController.h"
using namespace cugl;


void ResourceController::setResources(
    std::shared_ptr<cugl::scene2::Label> &bladeText, 
    std::shared_ptr<cugl::scene2::Label> &flourishText, 
    std::shared_ptr<cugl::scene2::Label> &lungeText,
    std::shared_ptr<cugl::scene2::Label> &brawnText,
    std::vector<int> &resources, int preview
    ){
    bladeText->setText(to_string(resources[0]));
    flourishText->setText(to_string(resources[1]));
    lungeText->setText(to_string(resources[2]));
    brawnText->setText(to_string(resources[3]));
    if (preview == 0) {
        bladeText->setForeground(Color4(124, 229, 227, 255));
    } else if (preview == 1){
        flourishText->setForeground(Color4(124, 229, 227, 255));
    } else if (preview == 2){
        lungeText->setForeground(Color4(124, 229, 227, 255));
    } else if (preview == 3){
        brawnText->setForeground(Color4(124, 229, 227, 255));
    } else {
        bladeText->setForeground(Color4(241,231,95, 255 ));
        flourishText->setForeground(Color4(241,231,95, 255 ));
        lungeText->setForeground(Color4(241,231,95, 255 ));
        brawnText->setForeground(Color4(241,231,95, 255 ));
    }



}  

void ResourceController::setResponseResources(
    std::map<int, Response> &responses, 
    int responseId, 
    int response, 
    std::shared_ptr<cugl::AssetManager> &assets,
    int mod){
     std::vector<int> cost = responses[responseId].getResources();
    //std::printf("curr: %d\n", _curr);
    if (responseId != 47 & responseId != 50 & responseId != 51 & responseId != 53 & responseId != 48){
        if (_curr == _free){
            _freeResponse = response;
            cost = {0,0,0,0};
        }
        _curr += 1;
    }
     int acc = 1;
     std::shared_ptr<cugl::scene2::NinePatch> responseResourcePointer;
     std::shared_ptr<cugl::scene2::Label> responseResourceAmountPointer;
     for (int i = 0; i < cost.size(); i++){
          if (cost[i] > 0){
               string path = "lab_response" + to_string(response) + "_up_resource" + to_string(acc);
               string amountPath = "lab_response" + to_string(response) + "_up_resource" + to_string(acc) + "_amount";
               responseResourcePointer = std::dynamic_pointer_cast<scene2::NinePatch>(assets->get<scene2::SceneNode>(path));
               responseResourceAmountPointer = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>(amountPath));
               responseResourcePointer->setVisible(true);
               responseResourceAmountPointer->setVisible(true);
               acc += 1;
               string resource = "brawn";
               if (i == 0){
                    resource = "blade";
               } else if (i == 1){
                    resource = "flourish";
               } else if (i == 2){
                    resource = "lunge";
               }
               responseResourcePointer->setTexture(assets->get<Texture>(resource));
               responseResourceAmountPointer->setText(to_string((cost[i] * mod)));
          }
     }
     for (int j = acc; j <= 4; j++){
          string path = "lab_response" + to_string(response) + "_up_resource" + to_string(j);
          string amountPath = "lab_response" + to_string(response) + "_up_resource" + to_string(j) + "_amount";
          responseResourcePointer = std::dynamic_pointer_cast<scene2::NinePatch>(assets->get<scene2::SceneNode>(path));
          responseResourceAmountPointer = std::dynamic_pointer_cast<scene2::Label>(assets->get<scene2::SceneNode>(amountPath));
          responseResourcePointer->setVisible(false);
          responseResourceAmountPointer->setVisible(false);
     }
     responseResourcePointer = nullptr;
     responseResourceAmountPointer = nullptr;
}

void ResourceController::setBurnText(
    Card &currentCard, 
    std::shared_ptr<cugl::scene2::Label> &burnText, 
    std::shared_ptr<cugl::AssetManager> &assets,
    std::shared_ptr<cugl::scene2::NinePatch> &burnTexture){
    for (int i = 0; i < 4; i ++){
        if (currentCard.getResource(i) > 0){
            burnText->setText(to_string(currentCard.getResource(i)));
            string resource = "brawn";
            if (i == 0){
                    resource = "blade";
            } else if (i == 1){
                    resource = "flourish";
            } else if (i == 2){
                    resource = "lunge";
            }
            burnTexture->setTexture(assets->get<Texture>(resource));
            return;
        }
    }
    burnText->setText(to_string(0));
    burnTexture->setTexture(assets->get<Texture>("flourish"));
}

void ResourceController::setDisplayCardBurnText(
    Card &displayCard, 
    std::shared_ptr<cugl::scene2::Label> &displayCardBurnText, 
    std::shared_ptr<cugl::AssetManager> &assets,
    std::shared_ptr<cugl::scene2::NinePatch> &displayCardBurnTexture){
    if (displayCard.getText() == "Incapacitated" || displayCard.getId() == 13 || displayCard.getId() == 14 || displayCard.getId() == 15){
        displayCardBurnTexture->setVisible(false);
    } else {
        displayCardBurnTexture->setVisible(true);
    }
    for (int i = 0; i < 4; i ++){
          if (displayCard.getResource(i) > 0){
               displayCardBurnText->setText(to_string(displayCard.getResource(i)));
               string resource = "brawn";
               if (i == 0){
                    resource = "blade";
               } else if (i == 1){
                    resource = "flourish";
               } else if (i == 2){
                    resource = "lunge";
               }
               displayCardBurnTexture->setTexture(assets->get<Texture>(resource));
              return;
          }
     }
    displayCardBurnText->setText(to_string(0));
    displayCardBurnTexture->setTexture(assets->get<Texture>("flourish"));
}

void ResourceController::setDisplayCardResponseType(
     Card &displayCard,
     std::shared_ptr<cugl::AssetManager> &assets,
     std::shared_ptr<cugl::scene2::NinePatch> &displayCardResponseType,
    std::shared_ptr<cugl::scene2::NinePatch> &displayCardResponseBrawn,
    bool brawler){
    string resource = displayCard.getResponseType();
    if (resource == "none"){
        displayCardResponseType->setVisible(false);
        displayCardResponseBrawn->setVisible(false);
    } else {
        displayCardResponseType->setTexture(assets->get<Texture>(resource));
        displayCardResponseType->setVisible(true);
        displayCardResponseBrawn->setVisible(false);
        if (brawler){
            if (resource != "brawn"){
                displayCardResponseBrawn->setVisible(true);
            }
        }
    }
}
