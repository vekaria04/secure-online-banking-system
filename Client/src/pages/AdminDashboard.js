// src/AdminDashboard.js
import React, { useEffect, useState } from "react";
import axios from "axios";
import {
  Container,
  Typography,
  Button,
  Box,
  Table,
  TableHead,
  TableBody,
  TableCell,
  TableRow,
  TextField,
  Dialog,
  DialogTitle,
  DialogContent,
  DialogActions,
} from "@mui/material";

function AdminDashboard() {
  const [users, setUsers] = useState([]);
  const [selectedUser, setSelectedUser] = useState(null);
  const [accounts, setAccounts] = useState([]);
  const [open, setOpen] = useState(false);

  const token = localStorage.getItem("token");

  const fetchUsers = async () => {
    const res = await axios.get("http://localhost:3001/admin/users", {
      headers: { Authorization: `Bearer ${token}` },
    });
    setUsers(res.data);
  };

  const deleteUser = async (userId) => {
    await axios.delete(`http://localhost:3001/admin/user/${userId}`, {
      headers: { Authorization: `Bearer ${token}` },
    });
    fetchUsers();
  };

  const openEditDialog = async (user) => {
    setSelectedUser({ ...user });
    const accRes = await axios.get(`http://localhost:3001/admin/user/${user.user_id}/accounts`, {
      headers: { Authorization: `Bearer ${token}` },
    });
    setAccounts(accRes.data);
    setOpen(true);
  };

  const handleSave = async () => {
    await axios.put(
      `http://localhost:3001/admin/user/${selectedUser.user_id}`,
      {
        name: selectedUser.name,
        email: selectedUser.email,
      },
      {
        headers: { Authorization: `Bearer ${token}` },
      }
    );
    setOpen(false);
    fetchUsers();
  };

  useEffect(() => {
    fetchUsers();
  }, []);

  return (
    <Container>
      <Typography variant="h4" gutterBottom>
        Admin Dashboard
      </Typography>
      <Table>
        <TableHead>
          <TableRow>
            <TableCell>User ID</TableCell>
            <TableCell>Name</TableCell>
            <TableCell>Email</TableCell>
            <TableCell>Actions</TableCell>
          </TableRow>
        </TableHead>
        <TableBody>
          {users.map((user) => (
            <TableRow key={user.user_id}>
              <TableCell>{user.user_id}</TableCell>
              <TableCell>{user.name}</TableCell>
              <TableCell>{user.email}</TableCell>
              <TableCell>
                <Button onClick={() => openEditDialog(user)} variant="outlined">
                  View/Edit
                </Button>
                <Button
                  onClick={() => deleteUser(user.user_id)}
                  variant="contained"
                  color="error"
                  sx={{ ml: 1 }}
                >
                  Delete
                </Button>
              </TableCell>
            </TableRow>
          ))}
        </TableBody>
      </Table>

      {/* Edit Dialog */}
      <Dialog open={open} onClose={() => setOpen(false)}>
        <DialogTitle>Edit User Info</DialogTitle>
        <DialogContent>
          <TextField
            label="Name"
            fullWidth
            margin="normal"
            value={selectedUser?.name || ""}
            onChange={(e) =>
              setSelectedUser({ ...selectedUser, name: e.target.value })
            }
          />
          <TextField
            label="Email"
            fullWidth
            margin="normal"
            value={selectedUser?.email || ""}
            onChange={(e) =>
              setSelectedUser({ ...selectedUser, email: e.target.value })
            }
          />
          <Typography variant="h6" mt={2}>
            Accounts
          </Typography>
          {accounts.map((acc) => (
            <Box key={acc.account_id} mt={1}>
              <Typography variant="body2">
                ID: {acc.account_id} | Type: {acc.account_type} | Balance: $
                {acc.balance}
              </Typography>
            </Box>
          ))}
        </DialogContent>
        <DialogActions>
          <Button onClick={handleSave}>Save</Button>
          <Button onClick={() => setOpen(false)}>Cancel</Button>
        </DialogActions>
      </Dialog>
    </Container>
  );
}

export default AdminDashboard;
