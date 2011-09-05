/*=========================================================================
 *
 *  Copyright Insight Software Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkBinaryImageToShiSparseLevelSetAdaptor.h"
#include "itkLabelImageToLabelMapFilter.h"

#include "itkLevelSetComparisonTestHelper.h"

template< unsigned int VDimension >
void GenerateSolution( char* ImagePath,
                       typename itk::ShiSparseLevelSetBase< VDimension >::Pointer Solution )
{
  typedef itk::ShiSparseLevelSetBase< VDimension >  LevelSetType;
  typedef typename LevelSetType::LabelObjectType    LabelObjectType;
  typedef typename LevelSetType::LabelMapType       LabelMapType;
  typedef typename LevelSetType::LayerIdType        LayerIdType;

  typedef itk::Image< LayerIdType, VDimension > ImageType;
  typedef itk::ImageFileReader< ImageType >     ImageReaderType;

  typename ImageReaderType::Pointer reader = ImageReaderType::New();
  reader->SetFileName( ImagePath );
  reader->Update();

  typedef itk::LabelImageToLabelMapFilter< ImageType, LabelMapType > I2LMFilterType;
  typename I2LMFilterType::Pointer i2lm = I2LMFilterType::New();
  i2lm->SetInput( reader->GetOutput() );
  i2lm->SetBackgroundValue( LevelSetType::PlusThreeLayer() );
  i2lm->Update();

  Solution->SetLabelMap( i2lm->GetOutput() );

  typedef typename LevelSetType::LayerType LayerType;

  LayerType layer = Solution->GetLayer( LevelSetType::MinusOneLayer() );

  typedef typename LevelSetType::IndexType IndexType;
  IndexType idx;
  idx[0]=29; idx[1]=68;
  layer[ idx ] =-1;
  idx[0]=29; idx[1]=69;
  layer[ idx ] =-1;
  idx[0]=29; idx[1]=70;
  layer[ idx ] =-1;
  idx[0]=29; idx[1]=71;
  layer[ idx ] =-1;
  idx[0]=29; idx[1]=72;
  layer[ idx ] =-1;
  idx[0]=29; idx[1]=73;
  layer[ idx ] =-1;
  idx[0]=29; idx[1]=74;
  layer[ idx ] =-1;
  idx[0]=29; idx[1]=75;
  layer[ idx ] =-1;
  idx[0]=29; idx[1]=76;
  layer[ idx ] =-1;
  idx[0]=30; idx[1]=62;
  layer[ idx ] =-1;
  idx[0]=30; idx[1]=63;
  layer[ idx ] =-1;
  idx[0]=30; idx[1]=64;
  layer[ idx ] =-1;
  idx[0]=30; idx[1]=65;
  layer[ idx ] =-1;
  idx[0]=30; idx[1]=66;
  layer[ idx ] =-1;
  idx[0]=30; idx[1]=67;
  layer[ idx ] =-1;
  idx[0]=30; idx[1]=77;
  layer[ idx ] =-1;
  idx[0]=30; idx[1]=78;
  layer[ idx ] =-1;
  idx[0]=30; idx[1]=79;
  layer[ idx ] =-1;
  idx[0]=30; idx[1]=80;
  layer[ idx ] =-1;
  idx[0]=30; idx[1]=81;
  layer[ idx ] =-1;
  idx[0]=30; idx[1]=82;
  layer[ idx ] =-1;
  idx[0]=31; idx[1]=59;
  layer[ idx ] =-1;
  idx[0]=31; idx[1]=60;
  layer[ idx ] =-1;
  idx[0]=31; idx[1]=61;
  layer[ idx ] =-1;
  idx[0]=31; idx[1]=83;
  layer[ idx ] =-1;
  idx[0]=31; idx[1]=84;
  layer[ idx ] =-1;
  idx[0]=31; idx[1]=85;
  layer[ idx ] =-1;
  idx[0]=32; idx[1]=56;
  layer[ idx ] =-1;
  idx[0]=32; idx[1]=57;
  layer[ idx ] =-1;
  idx[0]=32; idx[1]=58;
  layer[ idx ] =-1;
  idx[0]=32; idx[1]=86;
  layer[ idx ] =-1;
  idx[0]=32; idx[1]=87;
  layer[ idx ] =-1;
  idx[0]=32; idx[1]=88;
  layer[ idx ] =-1;
  idx[0]=33; idx[1]=54;
  layer[ idx ] =-1;
  idx[0]=33; idx[1]=55;
  layer[ idx ] =-1;
  idx[0]=33; idx[1]=89;
  layer[ idx ] =-1;
  idx[0]=33; idx[1]=90;
  layer[ idx ] =-1;
  idx[0]=34; idx[1]=52;
  layer[ idx ] =-1;
  idx[0]=34; idx[1]=53;
  layer[ idx ] =-1;
  idx[0]=34; idx[1]=91;
  layer[ idx ] =-1;
  idx[0]=34; idx[1]=92;
  layer[ idx ] =-1;
  idx[0]=35; idx[1]=50;
  layer[ idx ] =-1;
  idx[0]=35; idx[1]=51;
  layer[ idx ] =-1;
  idx[0]=35; idx[1]=93;
  layer[ idx ] =-1;
  idx[0]=35; idx[1]=94;
  layer[ idx ] =-1;
  idx[0]=36; idx[1]=48;
  layer[ idx ] =-1;
  idx[0]=36; idx[1]=49;
  layer[ idx ] =-1;
  idx[0]=36; idx[1]=95;
  layer[ idx ] =-1;
  idx[0]=36; idx[1]=96;
  layer[ idx ] =-1;
  idx[0]=37; idx[1]=47;
  layer[ idx ] =-1;
  idx[0]=37; idx[1]=97;
  layer[ idx ] =-1;
  idx[0]=38; idx[1]=46;
  layer[ idx ] =-1;
  idx[0]=38; idx[1]=98;
  layer[ idx ] =-1;
  idx[0]=39; idx[1]=45;
  layer[ idx ] =-1;
  idx[0]=39; idx[1]=99;
  layer[ idx ] =-1;
  idx[0]=40; idx[1]=43;
  layer[ idx ] =-1;
  idx[0]=40; idx[1]=44;
  layer[ idx ] =-1;
  idx[0]=40; idx[1]=100;
  layer[ idx ] =-1;
  idx[0]=40; idx[1]=101;
  layer[ idx ] =-1;
  idx[0]=41; idx[1]=42;
  layer[ idx ] =-1;
  idx[0]=41; idx[1]=102;
  layer[ idx ] =-1;
  idx[0]=42; idx[1]=41;
  layer[ idx ] =-1;
  idx[0]=42; idx[1]=103;
  layer[ idx ] =-1;
  idx[0]=43; idx[1]=40;
  layer[ idx ] =-1;
  idx[0]=43; idx[1]=104;
  layer[ idx ] =-1;
  idx[0]=44; idx[1]=40;
  layer[ idx ] =-1;
  idx[0]=44; idx[1]=104;
  layer[ idx ] =-1;
  idx[0]=45; idx[1]=39;
  layer[ idx ] =-1;
  idx[0]=45; idx[1]=105;
  layer[ idx ] =-1;
  idx[0]=46; idx[1]=38;
  layer[ idx ] =-1;
  idx[0]=46; idx[1]=106;
  layer[ idx ] =-1;
  idx[0]=47; idx[1]=37;
  layer[ idx ] =-1;
  idx[0]=47; idx[1]=107;
  layer[ idx ] =-1;
  idx[0]=48; idx[1]=36;
  layer[ idx ] =-1;
  idx[0]=48; idx[1]=108;
  layer[ idx ] =-1;
  idx[0]=49; idx[1]=36;
  layer[ idx ] =-1;
  idx[0]=49; idx[1]=108;
  layer[ idx ] =-1;
  idx[0]=50; idx[1]=35;
  layer[ idx ] =-1;
  idx[0]=50; idx[1]=109;
  layer[ idx ] =-1;
  idx[0]=51; idx[1]=35;
  layer[ idx ] =-1;
  idx[0]=51; idx[1]=109;
  layer[ idx ] =-1;
  idx[0]=52; idx[1]=34;
  layer[ idx ] =-1;
  idx[0]=52; idx[1]=110;
  layer[ idx ] =-1;
  idx[0]=53; idx[1]=34;
  layer[ idx ] =-1;
  idx[0]=53; idx[1]=110;
  layer[ idx ] =-1;
  idx[0]=54; idx[1]=33;
  layer[ idx ] =-1;
  idx[0]=54; idx[1]=111;
  layer[ idx ] =-1;
  idx[0]=55; idx[1]=33;
  layer[ idx ] =-1;
  idx[0]=55; idx[1]=111;
  layer[ idx ] =-1;
  idx[0]=56; idx[1]=32;
  layer[ idx ] =-1;
  idx[0]=56; idx[1]=112;
  layer[ idx ] =-1;
  idx[0]=57; idx[1]=32;
  layer[ idx ] =-1;
  idx[0]=57; idx[1]=112;
  layer[ idx ] =-1;
  idx[0]=58; idx[1]=32;
  layer[ idx ] =-1;
  idx[0]=58; idx[1]=112;
  layer[ idx ] =-1;
  idx[0]=59; idx[1]=31;
  layer[ idx ] =-1;
  idx[0]=59; idx[1]=113;
  layer[ idx ] =-1;
  idx[0]=60; idx[1]=31;
  layer[ idx ] =-1;
  idx[0]=60; idx[1]=113;
  layer[ idx ] =-1;
  idx[0]=61; idx[1]=31;
  layer[ idx ] =-1;
  idx[0]=61; idx[1]=113;
  layer[ idx ] =-1;
  idx[0]=62; idx[1]=30;
  layer[ idx ] =-1;
  idx[0]=62; idx[1]=114;
  layer[ idx ] =-1;
  idx[0]=63; idx[1]=30;
  layer[ idx ] =-1;
  idx[0]=63; idx[1]=114;
  layer[ idx ] =-1;
  idx[0]=64; idx[1]=30;
  layer[ idx ] =-1;
  idx[0]=64; idx[1]=114;
  layer[ idx ] =-1;
  idx[0]=65; idx[1]=30;
  layer[ idx ] =-1;
  idx[0]=65; idx[1]=114;
  layer[ idx ] =-1;
  idx[0]=66; idx[1]=30;
  layer[ idx ] =-1;
  idx[0]=66; idx[1]=114;
  layer[ idx ] =-1;
  idx[0]=67; idx[1]=30;
  layer[ idx ] =-1;
  idx[0]=67; idx[1]=114;
  layer[ idx ] =-1;
  idx[0]=68; idx[1]=29;
  layer[ idx ] =-1;
  idx[0]=68; idx[1]=115;
  layer[ idx ] =-1;
  idx[0]=69; idx[1]=29;
  layer[ idx ] =-1;
  idx[0]=69; idx[1]=115;
  layer[ idx ] =-1;
  idx[0]=70; idx[1]=29;
  layer[ idx ] =-1;
  idx[0]=70; idx[1]=115;
  layer[ idx ] =-1;
  idx[0]=71; idx[1]=29;
  layer[ idx ] =-1;
  idx[0]=71; idx[1]=115;
  layer[ idx ] =-1;
  idx[0]=72; idx[1]=29;
  layer[ idx ] =-1;
  idx[0]=72; idx[1]=115;
  layer[ idx ] =-1;
  idx[0]=73; idx[1]=29;
  layer[ idx ] =-1;
  idx[0]=73; idx[1]=115;
  layer[ idx ] =-1;
  idx[0]=74; idx[1]=29;
  layer[ idx ] =-1;
  idx[0]=74; idx[1]=115;
  layer[ idx ] =-1;
  idx[0]=75; idx[1]=29;
  layer[ idx ] =-1;
  idx[0]=75; idx[1]=115;
  layer[ idx ] =-1;
  idx[0]=76; idx[1]=29;
  layer[ idx ] =-1;
  idx[0]=76; idx[1]=115;
  layer[ idx ] =-1;
  idx[0]=77; idx[1]=30;
  layer[ idx ] =-1;
  idx[0]=77; idx[1]=114;
  layer[ idx ] =-1;
  idx[0]=78; idx[1]=30;
  layer[ idx ] =-1;
  idx[0]=78; idx[1]=114;
  layer[ idx ] =-1;
  idx[0]=79; idx[1]=30;
  layer[ idx ] =-1;
  idx[0]=79; idx[1]=114;
  layer[ idx ] =-1;
  idx[0]=80; idx[1]=30;
  layer[ idx ] =-1;
  idx[0]=80; idx[1]=114;
  layer[ idx ] =-1;
  idx[0]=81; idx[1]=30;
  layer[ idx ] =-1;
  idx[0]=81; idx[1]=114;
  layer[ idx ] =-1;
  idx[0]=82; idx[1]=30;
  layer[ idx ] =-1;
  idx[0]=82; idx[1]=114;
  layer[ idx ] =-1;
  idx[0]=83; idx[1]=31;
  layer[ idx ] =-1;
  idx[0]=83; idx[1]=113;
  layer[ idx ] =-1;
  idx[0]=84; idx[1]=31;
  layer[ idx ] =-1;
  idx[0]=84; idx[1]=113;
  layer[ idx ] =-1;
  idx[0]=85; idx[1]=31;
  layer[ idx ] =-1;
  idx[0]=85; idx[1]=113;
  layer[ idx ] =-1;
  idx[0]=86; idx[1]=32;
  layer[ idx ] =-1;
  idx[0]=86; idx[1]=112;
  layer[ idx ] =-1;
  idx[0]=87; idx[1]=32;
  layer[ idx ] =-1;
  idx[0]=87; idx[1]=112;
  layer[ idx ] =-1;
  idx[0]=88; idx[1]=32;
  layer[ idx ] =-1;
  idx[0]=88; idx[1]=112;
  layer[ idx ] =-1;
  idx[0]=89; idx[1]=33;
  layer[ idx ] =-1;
  idx[0]=89; idx[1]=111;
  layer[ idx ] =-1;
  idx[0]=90; idx[1]=33;
  layer[ idx ] =-1;
  idx[0]=90; idx[1]=111;
  layer[ idx ] =-1;
  idx[0]=91; idx[1]=34;
  layer[ idx ] =-1;
  idx[0]=91; idx[1]=110;
  layer[ idx ] =-1;
  idx[0]=92; idx[1]=34;
  layer[ idx ] =-1;
  idx[0]=92; idx[1]=110;
  layer[ idx ] =-1;
  idx[0]=93; idx[1]=35;
  layer[ idx ] =-1;
  idx[0]=93; idx[1]=109;
  layer[ idx ] =-1;
  idx[0]=94; idx[1]=35;
  layer[ idx ] =-1;
  idx[0]=94; idx[1]=109;
  layer[ idx ] =-1;
  idx[0]=95; idx[1]=36;
  layer[ idx ] =-1;
  idx[0]=95; idx[1]=108;
  layer[ idx ] =-1;
  idx[0]=96; idx[1]=36;
  layer[ idx ] =-1;
  idx[0]=96; idx[1]=108;
  layer[ idx ] =-1;
  idx[0]=97; idx[1]=37;
  layer[ idx ] =-1;
  idx[0]=97; idx[1]=107;
  layer[ idx ] =-1;
  idx[0]=98; idx[1]=38;
  layer[ idx ] =-1;
  idx[0]=98; idx[1]=106;
  layer[ idx ] =-1;
  idx[0]=99; idx[1]=39;
  layer[ idx ] =-1;
  idx[0]=99; idx[1]=105;
  layer[ idx ] =-1;
  idx[0]=100; idx[1]=40;
  layer[ idx ] =-1;
  idx[0]=100; idx[1]=104;
  layer[ idx ] =-1;
  idx[0]=101; idx[1]=40;
  layer[ idx ] =-1;
  idx[0]=101; idx[1]=104;
  layer[ idx ] =-1;
  idx[0]=102; idx[1]=41;
  layer[ idx ] =-1;
  idx[0]=102; idx[1]=103;
  layer[ idx ] =-1;
  idx[0]=103; idx[1]=42;
  layer[ idx ] =-1;
  idx[0]=103; idx[1]=102;
  layer[ idx ] =-1;
  idx[0]=104; idx[1]=43;
  layer[ idx ] =-1;
  idx[0]=104; idx[1]=44;
  layer[ idx ] =-1;
  idx[0]=104; idx[1]=100;
  layer[ idx ] =-1;
  idx[0]=104; idx[1]=101;
  layer[ idx ] =-1;
  idx[0]=105; idx[1]=45;
  layer[ idx ] =-1;
  idx[0]=105; idx[1]=99;
  layer[ idx ] =-1;
  idx[0]=106; idx[1]=46;
  layer[ idx ] =-1;
  idx[0]=106; idx[1]=98;
  layer[ idx ] =-1;
  idx[0]=107; idx[1]=47;
  layer[ idx ] =-1;
  idx[0]=107; idx[1]=97;
  layer[ idx ] =-1;
  idx[0]=108; idx[1]=48;
  layer[ idx ] =-1;
  idx[0]=108; idx[1]=49;
  layer[ idx ] =-1;
  idx[0]=108; idx[1]=95;
  layer[ idx ] =-1;
  idx[0]=108; idx[1]=96;
  layer[ idx ] =-1;
  idx[0]=109; idx[1]=50;
  layer[ idx ] =-1;
  idx[0]=109; idx[1]=51;
  layer[ idx ] =-1;
  idx[0]=109; idx[1]=93;
  layer[ idx ] =-1;
  idx[0]=109; idx[1]=94;
  layer[ idx ] =-1;
  idx[0]=110; idx[1]=52;
  layer[ idx ] =-1;
  idx[0]=110; idx[1]=53;
  layer[ idx ] =-1;
  idx[0]=110; idx[1]=91;
  layer[ idx ] =-1;
  idx[0]=110; idx[1]=92;
  layer[ idx ] =-1;
  idx[0]=111; idx[1]=54;
  layer[ idx ] =-1;
  idx[0]=111; idx[1]=55;
  layer[ idx ] =-1;
  idx[0]=111; idx[1]=89;
  layer[ idx ] =-1;
  idx[0]=111; idx[1]=90;
  layer[ idx ] =-1;
  idx[0]=112; idx[1]=56;
  layer[ idx ] =-1;
  idx[0]=112; idx[1]=57;
  layer[ idx ] =-1;
  idx[0]=112; idx[1]=58;
  layer[ idx ] =-1;
  idx[0]=112; idx[1]=86;
  layer[ idx ] =-1;
  idx[0]=112; idx[1]=87;
  layer[ idx ] =-1;
  idx[0]=112; idx[1]=88;
  layer[ idx ] =-1;
  idx[0]=113; idx[1]=59;
  layer[ idx ] =-1;
  idx[0]=113; idx[1]=60;
  layer[ idx ] =-1;
  idx[0]=113; idx[1]=61;
  layer[ idx ] =-1;
  idx[0]=113; idx[1]=83;
  layer[ idx ] =-1;
  idx[0]=113; idx[1]=84;
  layer[ idx ] =-1;
  idx[0]=113; idx[1]=85;
  layer[ idx ] =-1;
  idx[0]=114; idx[1]=62;
  layer[ idx ] =-1;
  idx[0]=114; idx[1]=63;
  layer[ idx ] =-1;
  idx[0]=114; idx[1]=64;
  layer[ idx ] =-1;
  idx[0]=114; idx[1]=65;
  layer[ idx ] =-1;
  idx[0]=114; idx[1]=66;
  layer[ idx ] =-1;
  idx[0]=114; idx[1]=67;
  layer[ idx ] =-1;
  idx[0]=114; idx[1]=77;
  layer[ idx ] =-1;
  idx[0]=114; idx[1]=78;
  layer[ idx ] =-1;
  idx[0]=114; idx[1]=79;
  layer[ idx ] =-1;
  idx[0]=114; idx[1]=80;
  layer[ idx ] =-1;
  idx[0]=114; idx[1]=81;
  layer[ idx ] =-1;
  idx[0]=114; idx[1]=82;
  layer[ idx ] =-1;
  idx[0]=115; idx[1]=68;
  layer[ idx ] =-1;
  idx[0]=115; idx[1]=69;
  layer[ idx ] =-1;
  idx[0]=115; idx[1]=70;
  layer[ idx ] =-1;
  idx[0]=115; idx[1]=71;
  layer[ idx ] =-1;
  idx[0]=115; idx[1]=72;
  layer[ idx ] =-1;
  idx[0]=115; idx[1]=73;
  layer[ idx ] =-1;
  idx[0]=115; idx[1]=74;
  layer[ idx ] =-1;
  idx[0]=115; idx[1]=75;
  layer[ idx ] =-1;
  idx[0]=115; idx[1]=76;
  layer[ idx ] =-1;

  layer = Solution->GetLayer( LevelSetType::PlusOneLayer() );
  idx[0]=28; idx[1]=68;
  layer[ idx ] =1;
  idx[0]=28; idx[1]=69;
  layer[ idx ] =1;
  idx[0]=28; idx[1]=70;
  layer[ idx ] =1;
  idx[0]=28; idx[1]=71;
  layer[ idx ] =1;
  idx[0]=28; idx[1]=72;
  layer[ idx ] =1;
  idx[0]=28; idx[1]=73;
  layer[ idx ] =1;
  idx[0]=28; idx[1]=74;
  layer[ idx ] =1;
  idx[0]=28; idx[1]=75;
  layer[ idx ] =1;
  idx[0]=28; idx[1]=76;
  layer[ idx ] =1;
  idx[0]=29; idx[1]=62;
  layer[ idx ] =1;
  idx[0]=29; idx[1]=63;
  layer[ idx ] =1;
  idx[0]=29; idx[1]=64;
  layer[ idx ] =1;
  idx[0]=29; idx[1]=65;
  layer[ idx ] =1;
  idx[0]=29; idx[1]=66;
  layer[ idx ] =1;
  idx[0]=29; idx[1]=67;
  layer[ idx ] =1;
  idx[0]=29; idx[1]=77;
  layer[ idx ] =1;
  idx[0]=29; idx[1]=78;
  layer[ idx ] =1;
  idx[0]=29; idx[1]=79;
  layer[ idx ] =1;
  idx[0]=29; idx[1]=80;
  layer[ idx ] =1;
  idx[0]=29; idx[1]=81;
  layer[ idx ] =1;
  idx[0]=29; idx[1]=82;
  layer[ idx ] =1;
  idx[0]=30; idx[1]=59;
  layer[ idx ] =1;
  idx[0]=30; idx[1]=60;
  layer[ idx ] =1;
  idx[0]=30; idx[1]=61;
  layer[ idx ] =1;
  idx[0]=30; idx[1]=83;
  layer[ idx ] =1;
  idx[0]=30; idx[1]=84;
  layer[ idx ] =1;
  idx[0]=30; idx[1]=85;
  layer[ idx ] =1;
  idx[0]=31; idx[1]=56;
  layer[ idx ] =1;
  idx[0]=31; idx[1]=57;
  layer[ idx ] =1;
  idx[0]=31; idx[1]=58;
  layer[ idx ] =1;
  idx[0]=31; idx[1]=86;
  layer[ idx ] =1;
  idx[0]=31; idx[1]=87;
  layer[ idx ] =1;
  idx[0]=31; idx[1]=88;
  layer[ idx ] =1;
  idx[0]=32; idx[1]=54;
  layer[ idx ] =1;
  idx[0]=32; idx[1]=55;
  layer[ idx ] =1;
  idx[0]=32; idx[1]=89;
  layer[ idx ] =1;
  idx[0]=32; idx[1]=90;
  layer[ idx ] =1;
  idx[0]=33; idx[1]=52;
  layer[ idx ] =1;
  idx[0]=33; idx[1]=53;
  layer[ idx ] =1;
  idx[0]=33; idx[1]=91;
  layer[ idx ] =1;
  idx[0]=33; idx[1]=92;
  layer[ idx ] =1;
  idx[0]=34; idx[1]=50;
  layer[ idx ] =1;
  idx[0]=34; idx[1]=51;
  layer[ idx ] =1;
  idx[0]=34; idx[1]=93;
  layer[ idx ] =1;
  idx[0]=34; idx[1]=94;
  layer[ idx ] =1;
  idx[0]=35; idx[1]=48;
  layer[ idx ] =1;
  idx[0]=35; idx[1]=49;
  layer[ idx ] =1;
  idx[0]=35; idx[1]=95;
  layer[ idx ] =1;
  idx[0]=35; idx[1]=96;
  layer[ idx ] =1;
  idx[0]=36; idx[1]=47;
  layer[ idx ] =1;
  idx[0]=36; idx[1]=97;
  layer[ idx ] =1;
  idx[0]=37; idx[1]=46;
  layer[ idx ] =1;
  idx[0]=37; idx[1]=98;
  layer[ idx ] =1;
  idx[0]=38; idx[1]=45;
  layer[ idx ] =1;
  idx[0]=38; idx[1]=99;
  layer[ idx ] =1;
  idx[0]=39; idx[1]=43;
  layer[ idx ] =1;
  idx[0]=39; idx[1]=44;
  layer[ idx ] =1;
  idx[0]=39; idx[1]=100;
  layer[ idx ] =1;
  idx[0]=39; idx[1]=101;
  layer[ idx ] =1;
  idx[0]=40; idx[1]=42;
  layer[ idx ] =1;
  idx[0]=40; idx[1]=102;
  layer[ idx ] =1;
  idx[0]=41; idx[1]=41;
  layer[ idx ] =1;
  idx[0]=41; idx[1]=103;
  layer[ idx ] =1;
  idx[0]=42; idx[1]=40;
  layer[ idx ] =1;
  idx[0]=42; idx[1]=104;
  layer[ idx ] =1;
  idx[0]=43; idx[1]=39;
  layer[ idx ] =1;
  idx[0]=43; idx[1]=105;
  layer[ idx ] =1;
  idx[0]=44; idx[1]=39;
  layer[ idx ] =1;
  idx[0]=44; idx[1]=105;
  layer[ idx ] =1;
  idx[0]=45; idx[1]=38;
  layer[ idx ] =1;
  idx[0]=45; idx[1]=106;
  layer[ idx ] =1;
  idx[0]=46; idx[1]=37;
  layer[ idx ] =1;
  idx[0]=46; idx[1]=107;
  layer[ idx ] =1;
  idx[0]=47; idx[1]=36;
  layer[ idx ] =1;
  idx[0]=47; idx[1]=108;
  layer[ idx ] =1;
  idx[0]=48; idx[1]=35;
  layer[ idx ] =1;
  idx[0]=48; idx[1]=109;
  layer[ idx ] =1;
  idx[0]=49; idx[1]=35;
  layer[ idx ] =1;
  idx[0]=49; idx[1]=109;
  layer[ idx ] =1;
  idx[0]=50; idx[1]=34;
  layer[ idx ] =1;
  idx[0]=50; idx[1]=110;
  layer[ idx ] =1;
  idx[0]=51; idx[1]=34;
  layer[ idx ] =1;
  idx[0]=51; idx[1]=110;
  layer[ idx ] =1;
  idx[0]=52; idx[1]=33;
  layer[ idx ] =1;
  idx[0]=52; idx[1]=111;
  layer[ idx ] =1;
  idx[0]=53; idx[1]=33;
  layer[ idx ] =1;
  idx[0]=53; idx[1]=111;
  layer[ idx ] =1;
  idx[0]=54; idx[1]=32;
  layer[ idx ] =1;
  idx[0]=54; idx[1]=112;
  layer[ idx ] =1;
  idx[0]=55; idx[1]=32;
  layer[ idx ] =1;
  idx[0]=55; idx[1]=112;
  layer[ idx ] =1;
  idx[0]=56; idx[1]=31;
  layer[ idx ] =1;
  idx[0]=56; idx[1]=113;
  layer[ idx ] =1;
  idx[0]=57; idx[1]=31;
  layer[ idx ] =1;
  idx[0]=57; idx[1]=113;
  layer[ idx ] =1;
  idx[0]=58; idx[1]=31;
  layer[ idx ] =1;
  idx[0]=58; idx[1]=113;
  layer[ idx ] =1;
  idx[0]=59; idx[1]=30;
  layer[ idx ] =1;
  idx[0]=59; idx[1]=114;
  layer[ idx ] =1;
  idx[0]=60; idx[1]=30;
  layer[ idx ] =1;
  idx[0]=60; idx[1]=114;
  layer[ idx ] =1;
  idx[0]=61; idx[1]=30;
  layer[ idx ] =1;
  idx[0]=61; idx[1]=114;
  layer[ idx ] =1;
  idx[0]=62; idx[1]=29;
  layer[ idx ] =1;
  idx[0]=62; idx[1]=115;
  layer[ idx ] =1;
  idx[0]=63; idx[1]=29;
  layer[ idx ] =1;
  idx[0]=63; idx[1]=115;
  layer[ idx ] =1;
  idx[0]=64; idx[1]=29;
  layer[ idx ] =1;
  idx[0]=64; idx[1]=115;
  layer[ idx ] =1;
  idx[0]=65; idx[1]=29;
  layer[ idx ] =1;
  idx[0]=65; idx[1]=115;
  layer[ idx ] =1;
  idx[0]=66; idx[1]=29;
  layer[ idx ] =1;
  idx[0]=66; idx[1]=115;
  layer[ idx ] =1;
  idx[0]=67; idx[1]=29;
  layer[ idx ] =1;
  idx[0]=67; idx[1]=115;
  layer[ idx ] =1;
  idx[0]=68; idx[1]=28;
  layer[ idx ] =1;
  idx[0]=68; idx[1]=116;
  layer[ idx ] =1;
  idx[0]=69; idx[1]=28;
  layer[ idx ] =1;
  idx[0]=69; idx[1]=116;
  layer[ idx ] =1;
  idx[0]=70; idx[1]=28;
  layer[ idx ] =1;
  idx[0]=70; idx[1]=116;
  layer[ idx ] =1;
  idx[0]=71; idx[1]=28;
  layer[ idx ] =1;
  idx[0]=71; idx[1]=116;
  layer[ idx ] =1;
  idx[0]=72; idx[1]=28;
  layer[ idx ] =1;
  idx[0]=72; idx[1]=116;
  layer[ idx ] =1;
  idx[0]=73; idx[1]=28;
  layer[ idx ] =1;
  idx[0]=73; idx[1]=116;
  layer[ idx ] =1;
  idx[0]=74; idx[1]=28;
  layer[ idx ] =1;
  idx[0]=74; idx[1]=116;
  layer[ idx ] =1;
  idx[0]=75; idx[1]=28;
  layer[ idx ] =1;
  idx[0]=75; idx[1]=116;
  layer[ idx ] =1;
  idx[0]=76; idx[1]=28;
  layer[ idx ] =1;
  idx[0]=76; idx[1]=116;
  layer[ idx ] =1;
  idx[0]=77; idx[1]=29;
  layer[ idx ] =1;
  idx[0]=77; idx[1]=115;
  layer[ idx ] =1;
  idx[0]=78; idx[1]=29;
  layer[ idx ] =1;
  idx[0]=78; idx[1]=115;
  layer[ idx ] =1;
  idx[0]=79; idx[1]=29;
  layer[ idx ] =1;
  idx[0]=79; idx[1]=115;
  layer[ idx ] =1;
  idx[0]=80; idx[1]=29;
  layer[ idx ] =1;
  idx[0]=80; idx[1]=115;
  layer[ idx ] =1;
  idx[0]=81; idx[1]=29;
  layer[ idx ] =1;
  idx[0]=81; idx[1]=115;
  layer[ idx ] =1;
  idx[0]=82; idx[1]=29;
  layer[ idx ] =1;
  idx[0]=82; idx[1]=115;
  layer[ idx ] =1;
  idx[0]=83; idx[1]=30;
  layer[ idx ] =1;
  idx[0]=83; idx[1]=114;
  layer[ idx ] =1;
  idx[0]=84; idx[1]=30;
  layer[ idx ] =1;
  idx[0]=84; idx[1]=114;
  layer[ idx ] =1;
  idx[0]=85; idx[1]=30;
  layer[ idx ] =1;
  idx[0]=85; idx[1]=114;
  layer[ idx ] =1;
  idx[0]=86; idx[1]=31;
  layer[ idx ] =1;
  idx[0]=86; idx[1]=113;
  layer[ idx ] =1;
  idx[0]=87; idx[1]=31;
  layer[ idx ] =1;
  idx[0]=87; idx[1]=113;
  layer[ idx ] =1;
  idx[0]=88; idx[1]=31;
  layer[ idx ] =1;
  idx[0]=88; idx[1]=113;
  layer[ idx ] =1;
  idx[0]=89; idx[1]=32;
  layer[ idx ] =1;
  idx[0]=89; idx[1]=112;
  layer[ idx ] =1;
  idx[0]=90; idx[1]=32;
  layer[ idx ] =1;
  idx[0]=90; idx[1]=112;
  layer[ idx ] =1;
  idx[0]=91; idx[1]=33;
  layer[ idx ] =1;
  idx[0]=91; idx[1]=111;
  layer[ idx ] =1;
  idx[0]=92; idx[1]=33;
  layer[ idx ] =1;
  idx[0]=92; idx[1]=111;
  layer[ idx ] =1;
  idx[0]=93; idx[1]=34;
  layer[ idx ] =1;
  idx[0]=93; idx[1]=110;
  layer[ idx ] =1;
  idx[0]=94; idx[1]=34;
  layer[ idx ] =1;
  idx[0]=94; idx[1]=110;
  layer[ idx ] =1;
  idx[0]=95; idx[1]=35;
  layer[ idx ] =1;
  idx[0]=95; idx[1]=109;
  layer[ idx ] =1;
  idx[0]=96; idx[1]=35;
  layer[ idx ] =1;
  idx[0]=96; idx[1]=109;
  layer[ idx ] =1;
  idx[0]=97; idx[1]=36;
  layer[ idx ] =1;
  idx[0]=97; idx[1]=108;
  layer[ idx ] =1;
  idx[0]=98; idx[1]=37;
  layer[ idx ] =1;
  idx[0]=98; idx[1]=107;
  layer[ idx ] =1;
  idx[0]=99; idx[1]=38;
  layer[ idx ] =1;
  idx[0]=99; idx[1]=106;
  layer[ idx ] =1;
  idx[0]=100; idx[1]=39;
  layer[ idx ] =1;
  idx[0]=100; idx[1]=105;
  layer[ idx ] =1;
  idx[0]=101; idx[1]=39;
  layer[ idx ] =1;
  idx[0]=101; idx[1]=105;
  layer[ idx ] =1;
  idx[0]=102; idx[1]=40;
  layer[ idx ] =1;
  idx[0]=102; idx[1]=104;
  layer[ idx ] =1;
  idx[0]=103; idx[1]=41;
  layer[ idx ] =1;
  idx[0]=103; idx[1]=103;
  layer[ idx ] =1;
  idx[0]=104; idx[1]=42;
  layer[ idx ] =1;
  idx[0]=104; idx[1]=102;
  layer[ idx ] =1;
  idx[0]=105; idx[1]=43;
  layer[ idx ] =1;
  idx[0]=105; idx[1]=44;
  layer[ idx ] =1;
  idx[0]=105; idx[1]=100;
  layer[ idx ] =1;
  idx[0]=105; idx[1]=101;
  layer[ idx ] =1;
  idx[0]=106; idx[1]=45;
  layer[ idx ] =1;
  idx[0]=106; idx[1]=99;
  layer[ idx ] =1;
  idx[0]=107; idx[1]=46;
  layer[ idx ] =1;
  idx[0]=107; idx[1]=98;
  layer[ idx ] =1;
  idx[0]=108; idx[1]=47;
  layer[ idx ] =1;
  idx[0]=108; idx[1]=97;
  layer[ idx ] =1;
  idx[0]=109; idx[1]=48;
  layer[ idx ] =1;
  idx[0]=109; idx[1]=49;
  layer[ idx ] =1;
  idx[0]=109; idx[1]=95;
  layer[ idx ] =1;
  idx[0]=109; idx[1]=96;
  layer[ idx ] =1;
  idx[0]=110; idx[1]=50;
  layer[ idx ] =1;
  idx[0]=110; idx[1]=51;
  layer[ idx ] =1;
  idx[0]=110; idx[1]=93;
  layer[ idx ] =1;
  idx[0]=110; idx[1]=94;
  layer[ idx ] =1;
  idx[0]=111; idx[1]=52;
  layer[ idx ] =1;
  idx[0]=111; idx[1]=53;
  layer[ idx ] =1;
  idx[0]=111; idx[1]=91;
  layer[ idx ] =1;
  idx[0]=111; idx[1]=92;
  layer[ idx ] =1;
  idx[0]=112; idx[1]=54;
  layer[ idx ] =1;
  idx[0]=112; idx[1]=55;
  layer[ idx ] =1;
  idx[0]=112; idx[1]=89;
  layer[ idx ] =1;
  idx[0]=112; idx[1]=90;
  layer[ idx ] =1;
  idx[0]=113; idx[1]=56;
  layer[ idx ] =1;
  idx[0]=113; idx[1]=57;
  layer[ idx ] =1;
  idx[0]=113; idx[1]=58;
  layer[ idx ] =1;
  idx[0]=113; idx[1]=86;
  layer[ idx ] =1;
  idx[0]=113; idx[1]=87;
  layer[ idx ] =1;
  idx[0]=113; idx[1]=88;
  layer[ idx ] =1;
  idx[0]=114; idx[1]=59;
  layer[ idx ] =1;
  idx[0]=114; idx[1]=60;
  layer[ idx ] =1;
  idx[0]=114; idx[1]=61;
  layer[ idx ] =1;
  idx[0]=114; idx[1]=83;
  layer[ idx ] =1;
  idx[0]=114; idx[1]=84;
  layer[ idx ] =1;
  idx[0]=114; idx[1]=85;
  layer[ idx ] =1;
  idx[0]=115; idx[1]=62;
  layer[ idx ] =1;
  idx[0]=115; idx[1]=63;
  layer[ idx ] =1;
  idx[0]=115; idx[1]=64;
  layer[ idx ] =1;
  idx[0]=115; idx[1]=65;
  layer[ idx ] =1;
  idx[0]=115; idx[1]=66;
  layer[ idx ] =1;
  idx[0]=115; idx[1]=67;
  layer[ idx ] =1;
  idx[0]=115; idx[1]=77;
  layer[ idx ] =1;
  idx[0]=115; idx[1]=78;
  layer[ idx ] =1;
  idx[0]=115; idx[1]=79;
  layer[ idx ] =1;
  idx[0]=115; idx[1]=80;
  layer[ idx ] =1;
  idx[0]=115; idx[1]=81;
  layer[ idx ] =1;
  idx[0]=115; idx[1]=82;
  layer[ idx ] =1;
  idx[0]=116; idx[1]=68;
  layer[ idx ] =1;
  idx[0]=116; idx[1]=69;
  layer[ idx ] =1;
  idx[0]=116; idx[1]=70;
  layer[ idx ] =1;
  idx[0]=116; idx[1]=71;
  layer[ idx ] =1;
  idx[0]=116; idx[1]=72;
  layer[ idx ] =1;
  idx[0]=116; idx[1]=73;
  layer[ idx ] =1;
  idx[0]=116; idx[1]=74;
  layer[ idx ] =1;
  idx[0]=116; idx[1]=75;
  layer[ idx ] =1;
  idx[0]=116; idx[1]=76;
  layer[ idx ] =1;
}

int itkBinaryImageToShiSparseLevelSetAdaptorTest( int argc, char* argv[] )
{
  if( argc < 3 )
    {
    std::cerr << "Missing Arguments" << std::endl;
    return EXIT_FAILURE;
    }

  const unsigned int Dimension = 2;

  typedef unsigned char InputPixelType;

  typedef itk::Image< InputPixelType, Dimension > InputImageType;
  typedef itk::ImageFileReader< InputImageType >  InputReaderType;

  InputReaderType::Pointer reader = InputReaderType::New();
  reader->SetFileName( argv[1] );
  try
    {
    reader->Update();
    }
  catch ( itk::ExceptionObject& err )
    {
    std::cout << err << std::endl;
    return EXIT_FAILURE;
    }
  InputImageType::Pointer input = reader->GetOutput();
  std::cout << "Input image read" << std::endl;

  typedef itk::BinaryImageToShiSparseLevelSetAdaptor< InputImageType >
    BinaryToSparseAdaptorType;

  BinaryToSparseAdaptorType::Pointer adaptor = BinaryToSparseAdaptorType::New();
  adaptor->SetInputImage( input );
  adaptor->Initialize();
  std::cout << "Finished converting to sparse format" << std::endl;

  typedef BinaryToSparseAdaptorType::LevelSetType     SparseLevelSetType;
  typedef SparseLevelSetType::LayerIdType             LayerIdType;

  SparseLevelSetType::Pointer sparseLevelSet = adaptor->GetSparseLevelSet();

  SparseLevelSetType::Pointer solution = SparseLevelSetType::New();
  GenerateSolution< Dimension >( argv[2], solution );

  bool comparison =
      itk::CompareShiLevelSets< Dimension >( sparseLevelSet,
                                             solution );

  if( !comparison )
    {
    std::cerr << "Results are too ifferent from the theoretical ones" << std::endl;
    return EXIT_FAILURE;
    }

  typedef BinaryToSparseAdaptorType::LevelSetOutputType LevelSetOutputType;

  typedef itk::Image< LevelSetOutputType, Dimension >   StatusImageType;
  StatusImageType::Pointer statusImage = StatusImageType::New();
  statusImage->SetRegions( input->GetLargestPossibleRegion() );
  statusImage->CopyInformation( input );
  statusImage->Allocate();
  statusImage->FillBuffer( 0 );

  typedef itk::ImageRegionIteratorWithIndex< StatusImageType > StatusIteratorType;
  StatusIteratorType sIt( statusImage, statusImage->GetLargestPossibleRegion() );
  sIt.GoToBegin();

  StatusImageType::IndexType idx;

  while( !sIt.IsAtEnd() )
    {
    idx = sIt.GetIndex();
    sIt.Set( sparseLevelSet->Evaluate( idx ) );
    ++sIt;
    }

  typedef itk::ImageFileWriter< StatusImageType >     StatusWriterType;
  StatusWriterType::Pointer writer = StatusWriterType::New();
  writer->SetFileName( argv[2] );
  writer->SetInput( statusImage );

  try
    {
    writer->Update();
    }
  catch ( itk::ExceptionObject& err )
    {
    std::cout << err << std::endl;
    return EXIT_FAILURE;
    }

  for( LayerIdType lyr = sparseLevelSet->MinusOneLayer();
      lyr <= sparseLevelSet->PlusOneLayer(); lyr += 2 )
    {
    SparseLevelSetType::LayerType layer = sparseLevelSet->GetLayer( lyr );
    SparseLevelSetType::LayerIterator lIt = layer.begin();

    std::cout << "*** " << static_cast< int >( lyr ) << " ***" <<std::endl;

    while( lIt != layer.end() )
      {
      std::cout << lIt->first << std::endl;
      ++lIt;
      }
    std::cout << std::endl;
    }

  typedef itk::LabelObject< unsigned long, 2 >  LabelObjectType;
  typedef LabelObjectType::Pointer              LabelObjectPointer;

  LabelObjectPointer labelObject = LabelObjectType::New();
  LabelObjectPointer labelObjectSrc = sparseLevelSet->GetAsLabelObject<unsigned long>();
  labelObject->CopyAllFrom( labelObjectSrc );
  labelObject->SetLabel( sparseLevelSet->PlusOneLayer() );

  labelObject->Optimize();
  std::cout << labelObject->Size() << std::endl;

  return EXIT_SUCCESS;
}
