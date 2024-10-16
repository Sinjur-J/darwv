#include <vtkColorTransferFunction.h>
#include <vtkNamedColors.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkStructuredGrid.h>
#include <vtkStructuredGridGeometryFilter.h>
#include <vtkStructuredGridReader.h>

int main(int argc, char *argv[]) {
    vtkNew<vtkNamedColors> colors;

    // Verify input arguments
    if (argc != 2) {
        std::cout << "Usage: " << argv[0]
                  << " Filename(.vtk) e.g. SampleStructGrid.vtk" << std::endl;
        return EXIT_FAILURE;
    }

    std::string inputFilename = argv[1];

    // Read the file
    vtkNew<vtkStructuredGridReader> reader;
    reader->SetFileName(inputFilename.c_str());
    reader->Update();

    vtkNew<vtkStructuredGridGeometryFilter> geometryFilter;
    geometryFilter->SetInputConnection(reader->GetOutputPort());
    geometryFilter->Update();

    std::array<double, 2> scalarRange;
    reader->GetOutput()->GetScalarRange(scalarRange.data());

    vtkNew<vtkColorTransferFunction> ctf;
    ctf->AddRGBPoint(scalarRange[0], 0.0, 0.0, 1.0);
    ctf->AddRGBPoint((scalarRange[0] + scalarRange[1]) / 2.0, 1.0, 1.0, 1.0);
    ctf->AddRGBPoint(scalarRange[1], 1.0, 0.0, 0.0);

    // Visualize
    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputConnection(geometryFilter->GetOutputPort());
    mapper->SetLookupTable(ctf);

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);

    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    renderWindow->SetWindowName("StructuredGridReader");

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(renderWindow);

    renderer->AddActor(actor);
    renderer->SetBackground(colors->GetColor3d("SlateGray").GetData());

    renderWindow->Render();
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}
