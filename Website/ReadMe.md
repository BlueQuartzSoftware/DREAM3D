# Instructions #

    [] $ use_anaconda
    [] $ conda activate dream3d
    [] $ mkdocs build -d dream3d-wp.bluequartz.net
  
  ssh into thor.bluequartz.net and navigate to the dream3d-wp.bluequartz.net directory.

      sudo scp -r mjackson@jolder.bluequartz.net:/Users/mjackson/DREAM3D-Dev/DREAM3D/Website/dream3d-wp.bluequartz.net/* .

That should update the website.

