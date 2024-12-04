import { useForm } from "@mantine/form";
import { TextInput } from "@mantine/core";
import { Link, Navigate, useNavigate } from "react-router-dom";
import { toast } from "sonner";
import { useState } from "react";
import Button from "../components/button";

const SignupOrganizer = () => {
  const [loading, setLoading] = useState(false);

  const navigate = useNavigate();

  const onSubmit = async (values) => {
    setLoading(true);
    await fetch(`${process.env.REACT_APP_FETCH_URL}/signup_organiser`, {
      method: "POST",
      headers: {
        "Content-Type": "application/json",
      },
      body: JSON.stringify(values),
    })
      .then(async (res) => {
        let jsonData = await res.json();
        if (!res.ok) {
          toast.error(jsonData.message);
        } else {
          toast.success(jsonData.message);
          navigate("/login");
        }
        setLoading(false);
      })
      .catch((e) => {
        toast.error(e.message);
        setLoading(false);
      });
  };

  const form = useForm({
    initialValues: {
      email: "",
      name: "",
      phone: "",
      password: "",
      confirm_password: "",
    },
    validate: (values) => ({
      email: /^\S+@\S+$/.test(values.email) ? null : "Invalid email",
      name: values.name.trim().length > 3 ? null : "Atleast 3 characters",
      phone: /^\d{10}$/.test(values.phone) ? null : "Invalid phone",
      password: values.password.length > 5 ? null : "Atleast 6 characters",
      confirm_password:
        values.password === values.confirm_password
          ? null
          : "Passwords do not match",
    }),
  });

  if (localStorage.getItem("access_token")) {
    return <Navigate to="/dashboard" replace />;
  }

  return (
    <div className="h-screen flex items-center justify-center bg-gradient-to-br from-blue-800 via bg-blue-500 to-blue-300">
      <div className="bg-white rounded-xl px-8 py-6 flex flex-col gap-4 w-[350px] sm:w-[450px]">
        <div className="flex flex-col">
          <span className="text-2xl font-semibold">Organizer Signup</span>
          <span className="text-sm text-neutral-500 font-semibold">
            Create an organizer account
          </span>
        </div>
        <form onSubmit={form.onSubmit((values) => onSubmit(values))}>
          <TextInput
            label="Email"
            placeholder="Email"
            {...form.getInputProps("email")}
          />
          <div className="flex gap-4">
            <TextInput
              mt="xs"
              label="Name"
              placeholder="Name"
              {...form.getInputProps("name")}
            />
            <TextInput
              mt="xs"
              label="Phone"
              placeholder="10 digits"
              {...form.getInputProps("phone")}
            />
          </div>
          <TextInput
            mt="xs"
            type="password"
            label="Password"
            placeholder="Password"
            {...form.getInputProps("password")}
          />
          <TextInput
            mt="xs"
            type="password"
            label="Confirm Password"
            placeholder="Confirm Password"
            {...form.getInputProps("confirm_password")}
          />
          <div className="flex justify-end gap-4">
            <Link
              to="/signup"
              className="bg-blue-500 px-4 py-2 rounded-md text-white font-semibold text-sm mt-6"
            >
              Prev
            </Link>
            <Button
              type="submit"
              className="bg-blue-500 px-4 py-2 rounded-md text-white font-semibold text-sm mt-6"
              text="Submit"
              loading={loading}
            />
          </div>
        </form>
        <Link
          to="/login"
          className="text-sm hover:underline text-center w-fit m-auto"
        >
          Already have an account?
        </Link>
      </div>
    </div>
  );
};

export default SignupOrganizer;
