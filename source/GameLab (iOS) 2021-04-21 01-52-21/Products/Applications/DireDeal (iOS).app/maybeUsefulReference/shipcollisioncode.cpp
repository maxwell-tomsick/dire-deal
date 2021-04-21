// void collisions::checkForCollision(const std::shared_ptr<Ship>& ship1, const std::shared_ptr<Ship>& ship2, const cugl::Rect bounds) {
//     for(int i = -1; i <=1; i++){
//         for(int j = -1; j <=1; j++){
//             // Calculate the normal of the (possible) point of collision
//             Vec2 norm = ship1->getPosition()+ Vec2((bounds.size.width) * i,(bounds.size.height) * j)-ship2->getPosition();
//             float distance = norm.length();
//             float impactDistance = (ship1->getRadius() + ship2->getRadius());
//             norm.normalize();

//             // If this normal is too small, there was a collision
//             if (distance < impactDistance) {
//                 // "Roll back" time so that the ships are barely touching (e.g. point of impact).
//                 Vec2 temp = norm * ((impactDistance - distance) / 2);
//                 ship1->setPosition(ship1->getPosition()+temp);
//                 ship2->setPosition(ship2->getPosition()-temp);

//                 // Now it is time for Newton's Law of Impact.
//                 // Convert the two velocities into a single reference frame
//                 Vec2 vel = ship1->getVelocity()-ship2->getVelocity();

//                 // Compute the impulse (see Essential Math for Game Programmers)
//                 float impulse = (-(1 + COLLISION_COEFF) * norm.dot(vel)) /
//                                 (norm.dot(norm) * (1 / ship1->getMass() + 1 / ship2->getMass()));

//                 // Change velocity of the two ships using this impulse
//                 temp = norm * (impulse/ship1->getMass());
//                 ship1->setVelocity(ship1->getVelocity()+temp);

//                 temp = norm * (impulse/ship2->getMass());
//                 ship2->setVelocity(ship2->getVelocity()-temp);
//             }
//         }
//     }
// }




// /**
//  *  Handles collisions between a ship and a photon.
//  *
//  *  A collision bounces the hit ship back and destroys the photon (e.g. age
//  *  is set to the maximum).
//  *
//  *  @param ship     The ship in the candidate collision
//  *  @param photons  The photons in the candidate collision
//  */
// void collisions::checkForCollision(const std::shared_ptr<Ship>& ship, const std::shared_ptr<PhotonQueue>& photons, const cugl::Rect bounds) {
//     // Get the photon size from the texture
//     if (ship->getJumping() > 20){
//         return;
//     }
//     auto texture = photons->getTexture();
//     float pradius = 0;
//     if (texture != nullptr) {
//         pradius = std::max(texture->getWidth(),texture->getHeight())/2.0f;
//     }
    
//     for(size_t ii = 0; ii < photons->size(); ii++) {
//         for(int i = -1; i <=1; i++){
//             for(int j = -1; j <=1; j++){
//                 // This returns a reference
//                 Photon* photon = photons->get(ii);
//                 // We are immune to our own photons
//                 if (photon != nullptr && photon->ship != ship->getSID()) {
//                     Vec2 norm = ship->getPosition()+ Vec2((bounds.size.width) * i,(bounds.size.height) * j)-photon->pos;
//                     float distance = norm.length();
//                     float impactDistance = (ship->getRadius() + pradius*photon->scale);
//                     norm.normalize();

//                     // If this normal is too small, there was a collision
//                     if (distance < impactDistance) {
//                         // "Roll back" time so that the ships are barely touching (e.g. point of impact).
//                         Vec2 temp = norm * ((impactDistance - distance) / 2);
//                         ship->setPosition(ship->getPosition()+temp);

//                         // Now it is time for Newton's Law of Impact.
//                         // Convert the two velocities into a single reference frame
//                         Vec2 vel = ship->getVelocity()-photon->vel;

//                         // Compute the impulse (see Essential Math for Game Programmers)
//                         float impulse = (-(1 + COLLISION_COEFF) * norm.dot(vel)) /
//                                         (norm.dot(norm) * (1 / ship->getMass() + 1 / PHOTON_MASS));

//                         // Change velocity of the two ships using this impulse
//                         temp = norm * (impulse/ship->getMass());
//                         ship->setVelocity(ship->getVelocity()+temp);

//                         // Destroy the photon
//                         photon->destroy();
//                     }
//                 }
//             }
//         }
//     }

// }

// /**
//  * Nudge the ship to ensure it does not do out of view.
//  *
//  * This code bounces the ship off walls.  You will replace it as part of
//  * the lab.
//  *
//  * @param ship      They player's ship which may have collided
//  * @param bounds    The rectangular bounds of the playing field
//  */
// void collisions::checkInBounds(const std::shared_ptr<Ship>& ship, const Rect bounds) {
//     // UNLIKE Java, these are values, not references
//     //Vec2 vel = ship->getVelocity();
//     Vec2 pos = ship->getPosition();
    
//     //Ensure the ship doesn't go out of view. Bounce off walls.
//     if (pos.x <= bounds.origin.x) {
//         //vel.x = -vel.x;
//         pos.x = bounds.size.width+bounds.origin.x-1.0f;
//         //ship->setVelocity(vel);
//         ship->setPosition(pos);
//     } else if (pos.x >= bounds.size.width+bounds.origin.x) {
//         //vel.x = -vel.x;
//         pos.x = bounds.origin.x;
//         //ship->setVelocity(vel);
//         ship->setPosition(pos);
//     }

//     if (pos.y <= bounds.origin.y) {
//         //vel.y = -vel.y;
//         //pos.y = bounds.origin.y;
//         pos.y = bounds.size.height+bounds.origin.y-1.0f;
//         //ship->setVelocity(vel);
//         ship->setPosition(pos);
//     } else if (pos.y >= bounds.size.height+bounds.origin.y) {
//         //vel.y = -vel.y;
//         pos.y = bounds.origin.y;
//         //ship->setVelocity(vel);
//         ship->setPosition(pos);
//     }

// }

